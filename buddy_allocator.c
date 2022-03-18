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
  int list_items=1<<(num_levels+1); // maximum number of allocations, used to determine the max list items
  int list_alloc_size=(sizeof(BuddyListItem)+sizeof(int))*list_items;
  return list_alloc_size;
}

void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* buffer,
                         int buffer_size,
                         char* memory,
                         int min_bucket_size){

  // we need room also for level 0
  alloc->num_levels=num_levels;
  alloc->memory=memory;
  alloc->min_bucket_size=min_bucket_size;
  alloc->bitmap= (BitMap) buffer;

  assert (num_levels<MAX_LEVELS);
  // we need enough memory to handle internal structures
  assert (buffer_size>=BuddyAllocator_calcSize(num_levels));


  int n_bits = (1 << num_levels)-1; //left shift di 1 == 2^num_levels


  //inizializzo la BitMap
  BitMap_init(alloc->bitmap,n_bits,buffer);
  printf("Numero bit della bitmap: %d\n", alloc->tree->num_bits);

  //impongo che nella bitmap sia disponibile solo il primo livello
  BitMap_setBit(alloc->bitmap,0,1);
  for(int i = 1; i < n_bits; i++)
	  BitMap_setBit(alloc->bitmap,i,0);


  }
  // da fare fzaux e malloc
  int BuddyAllocator_aux(BuddyAllocator* alloc, int lvl){
    if (level<1)
  		return 0;
    assert(level <= alloc->num_levels);

    //in un albero binario il primo indice di un livello
    //e il suo numero massimo di nodi a quel livello
    //si ricavano nello stesso modo
    int first_idx = 1 << (lvl - 1);
    int n_bud= first_idx;

    for (int i = 0; i < n_bud; i++) {
      if(BitMap_bit(alloc->bitmap, first_idx + i - 1)){ //-1 perche la bitmap parte da 0
        //se il buddy e libero ritorno il suo indice
        //e setto il bit nella bitmap come unavailable
        BitMap_setBit(alloc->bitmap, first_idx + i -1 ,0);
        return first_idx+i;
      }
    }

    // da fare caso in cui non ci siano buddy available al livello lvl1
  }

//per la malloc ho bisogno di creare una funzione ausiliaria che
//mi restituisca dei buddy consoni alle richieste
//effettuate , per poi poterli utilizzare nella malloc vera e propria


//vale altrettanto per la free : posso creare una funzione che
//mi 'riunisca' la memoria precedentemente partizionata
