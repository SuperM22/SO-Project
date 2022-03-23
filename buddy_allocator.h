#pragma once
#include "bitmap.h"

#define MAX_LEVELS 16

typedef struct  {
  BitMap bitmap;
  int num_levels;
  char* buffer; // the buffer area to be managed
  int buffer_size;
  int min_bucket_size; // the minimum page of RAM that can be returned
} BuddyAllocator;

//semplice funzione di stampa per la bitmap
void BitMap_print(BitMap* bit_map);

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* buffer, //buffer allocatore
                         int buffer_size,
                         char* memory, //buffer per la bitmap
                         int min_bucket_size);

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);


//ritorna l indice nella bitmap del buddy libero
//0 se non vi e memoria disponibile
int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int lvl);
//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
