#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
};

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
  alloc->buffer=buffer;
  alloc->min_bucket_size=min_bucket_size;
  //alloc->bitmap= (BitMap*) memory;

  assert (num_levels<MAX_LEVELS);
  // we need enough memory to handle internal structures
  assert (buffer_size>=BuddyAllocator_calcSize(num_levels));


  int n_bits = (1 << (num_levels + 1)) -1;


  //inizializzo la BitMap
  BitMap_init(&alloc->bitmap,n_bits,memory);
  printf("Numero bit della bitmap: %d\n", alloc->bitmap.num_bits);

  //impongo che nella bitmap sia disponibile solo il primo livello
  /*BitMap_setBit(&alloc->bitmap,0,1);
  for(int i = 1; i < n_bits; i++)
	  BitMap_setBit(&alloc->bitmap,i,0);*/


  }

  int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int lvl){
    if (lvl<1)
  		return 0;
    assert(lvl <= alloc->num_levels);

    //in un albero binario il primo indice di un livello
    //e il suo numero massimo di nodi a quel livello
    //si ricavano nello stesso modo
    int first_idx = 1 << (lvl - 1);
    int n_bud= first_idx;

    for (int i = 0; i < n_bud; i++) {
      if(BitMap_bit(&alloc->bitmap, first_idx + i - 1)){ //-1 perche la bitmap parte da 0
        //se il buddy e libero ritorno il suo indice
        //e setto il bit nella bitmap come unavailable
        BitMap_setBit(&alloc->bitmap, first_idx + i -1 ,0);
        return first_idx+i;
      }
    }

    //  caso in cui non ci siano buddy available al livello lvl
    //cerco nel livello del genitore
    int final_idx = BuddyAllocator_getBuddy(alloc, levelIdx(parentIdx(first_idx))) *2;

    if(!final_idx) return 0; //se non vi e memoria disponibile ritorno 0


    int mem_idx = buddyIdx(final_idx);


    BitMap_setBit(&alloc->bitmap, mem_idx-1, 1);


    return final_idx;

  }


  void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size){
    int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
    int  level=floor(log2(mem_size/(size+4)));//determino il livello della pagina

    //se il livello e troppo piccolo lo paddiamo al massimo
    if (level>alloc->num_levels)
    level=alloc->num_levels;

    printf("Requested: %d bytes, level %d \n",
         size, level);

    int idx_free = BuddyAllocator_getBuddy(alloc,level);
    //cerco un buddy libero

    if(!idx_free) return 0;

    //manca la parte della memoria
    int offset = idx_free - (1<<(level-1)); //offset del nodo idxfree al livello level
    int tot_memory= alloc->min_bucket_size * (1<<(alloc->num_levels-1)); //memoria massima

    char* buf = alloc->buffer + tot_memory / (1 << level-1) * offset;

    *((int*)buf) = idx_free; //scrivo l indice nella prima parte della memoria allocata

    printf("Indice:%d. Indirizzo:%p \n",idx_free,buf);

    return buf + 4; //sizeof int

  }



//vale altrettanto per la free : posso creare una funzione che
//mi 'riunisca' la memoria precedentemente partizionata
