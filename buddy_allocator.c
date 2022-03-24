#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"



//semplice funzione di stampa
void BitMap_print(BitMap* bit_map){

  for (int i=0; i<bit_map->num_bits; i++){
    if (i==firstIdx(levelIdx(i))){
      printf ("level %d: ", levelIdx(i));
      for (int k=bit_map->num_bits>>(levelIdx(i)+1);k>0; k--){
        printf(" ");
      }
    }
    if (levelIdx(i)!=levelIdx(i+1)){
      printf("%d ", BitMap_bit(bit_map,i));
      printf("\n");
    }
    else{
     printf("%d ", BitMap_bit(bit_map,i));
     for (int k=bit_map->num_bits>>(levelIdx(i)+1);k>0; k--){
        printf("  ");
      }
    }
  }
  printf("\n");

  }

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
  return (int)floor(log2(idx)) + 1;
};

int firstIdx(int lvl){
  return (1 << lvl) - 1;

}

int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

int parentIdx(int idx){
  return idx/2;
}

int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}

// computes the size in bytes for the allocator
int BuddyAllocator_calcSize(int num_levels) {
  int n_bits= 1<<(num_levels) - 1; //bit utilizzati
  return BitMap_getBytes(n_bits) + sizeof(BitMap);
}

void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* buffer,
                         int buffer_size,
                         char* memory,
                         int min_bucket_size){

  // we need room also for level 0
  alloc->num_levels=num_levels;
  alloc->buffer=memory;
  alloc->min_bucket_size=min_bucket_size;
  alloc->bitmap= (BitMap*) buffer;

  assert (num_levels<MAX_LEVELS);
  // we need enough memory to handle internal structures
  assert (buffer_size>=BuddyAllocator_calcSize(num_levels));


  int n_bits = (1 << num_levels ) - 1;


  //inizializzo la BitMap
  BitMap_init(alloc->bitmap,n_bits,buffer+sizeof(BitMap));
  printf("Numero bit della bitmap: %d\n", alloc->bitmap->num_bits);

  //impongo che nella bitmap sia disponibile solo il primo livello
  BitMap_setBit(alloc->bitmap,0,1);
  for(int i = 1; i < n_bits; i++)
	  BitMap_setBit(alloc->bitmap,i,0);


  }

  int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int lvl){
    //printf("%d   livello\n",lvl );
    if (lvl<1)
  		return 0;
    assert(lvl <= alloc->num_levels);

    //in un albero binario il primo indice di un livello
    //e il suo numero massimo di nodi a quel livello
    //si ricavano nello stesso modo
    int first_idx = 1 << (lvl - 1);
    int n_bud= 1 << (lvl - 1);

    //printf("%d, %d 1st index e num buddy\n", first_idx,n_bud);
    for (int i = 0; i < n_bud; i++) {
      //printf("%d indice\n",first_idx+i -1 );
      if(BitMap_bit(alloc->bitmap, first_idx + i - 1)){ //-1 perche la bitmap parte da 0
        //se il buddy e libero ritorno il suo indice
        //e setto il bit nella bitmap come unavailable

        BitMap_setBit(alloc->bitmap, first_idx + i - 1, 0);
        return first_idx + i;
      }
    }

    //  caso in cui non ci siano buddy available al livello lvl
    //cerco nel livello del genitore
    int final_idx = BuddyAllocator_getBuddy(alloc, levelIdx(parentIdx(first_idx))) *2;
    //printf("%d\n",final_idx );
    if(!final_idx) return 0; //se non vi e memoria disponibile ritorno 0


    int mem_idx = buddyIdx(final_idx);


    BitMap_setBit(alloc->bitmap, mem_idx - 1, 1);


    return final_idx;
}


  void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size){
    int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
    int  level=floor(log2(mem_size/(size+sizeof(int))));//determino il livello della pagina

    //se il livello e troppo piccolo lo paddiamo al massimo
    if (level>alloc->num_levels)
      level=alloc->num_levels;

    printf("Requested: %d bytes, level %d \n",
         size, level);

    int idx_free = BuddyAllocator_getBuddy(alloc,level);
    //cerco un buddy libero
    //printf("Ci sto\n" );
    if(!idx_free) return 0;


    int offset = idx_free - (1 << (level-1));
    //offset del nodo idxfree al livello level
    int tot_memory= alloc->min_bucket_size * (1 << (alloc->num_levels-1));
     //memoria massima

    char* buf = alloc->buffer + tot_memory / (1 << (level-1)) * offset;

    *((int*)buf) = idx_free;
     //scrivo l indice nella prima parte della memoria allocata
    //printf("Arrivato\n" );
    printf("INFO ALLOCAZIONE Indice: %d Indirizzo: %p\n", idx_free, buf);


    return (buf + sizeof(int));
}
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int idx){
  int buddy= buddyIdx(idx);
  //check se il buddy e libero
  if(idx!= 1 && BitMap_bit(alloc->bitmap,buddy-1)){
    //se e libero unisco i buddy settando il bit a zero
    BitMap_setBit(alloc->bitmap,buddy -1 ,0);
    printf("Unisco i buddy: %d e %d\n",idx,buddy);
    //devo anche vedere se posso unire buddy del livello superiore
    BuddyAllocator_releaseBuddy(alloc,parentIdx(idx));
    }

    else {
      BitMap_setBit(alloc->bitmap,idx-1,1);
    }
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){
  //riprendo l indice del buddy che ho precedentemente messo nei primi 4
 int buddy = *(int*)((char*)mem - sizeof(int));

 assert(buddy <(1<<alloc->num_levels));

 printf("Libero l'indirizzo: %p \n",mem);

 //side effect sulla BitMap
 BuddyAllocator_releaseBuddy(alloc,buddy);

}
