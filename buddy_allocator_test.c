#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 102400
#define BUDDY_LEVELS 9
#define MEMORY_SIZE (1024*1024) //1 mega di memoria
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))

char buffer[BUFFER_SIZE]; // 100 Kb buffer to handle memory should be enough
char memory[MEMORY_SIZE];

BuddyAllocator alloc;


/*static void Bitmap_print2(BitMap* map){
  int i;
	printf("Bitmap: (");
	for(i = 0; i < map->num_bits; i++)
		printf(" %d,", BitMap_bit(map, i));
	printf(")\n\n");
}

static void print_mem(){
  for(int i =0 ; i<MEMORY_SIZE;i++){
    printf("%d, ",memory[i]);
  }
  printf("\n" );
}*/

int main(int argc, char** argv) {

  //1 we see if we have enough memory for the buffers
  int req_size=BuddyAllocator_calcSize(BUDDY_LEVELS);
  printf("size requested for initialization: %d/BUFFER_SIZE\n", req_size);
  printf("MIN_BUCKET_SIZE: %d\n", MIN_BUCKET_SIZE );
  //2 we initialize the allocator
  printf("init... ");
  BuddyAllocator_init(&alloc, BUDDY_LEVELS,
                      buffer,
                      BUFFER_SIZE,
                      memory,
                      MIN_BUCKET_SIZE);
  printf("DONE\n");

  void* p1=BuddyAllocator_malloc(&alloc, 100);

  void* p2=BuddyAllocator_malloc(&alloc, 100);
  //BitMap_print(&alloc);
  void* p3=BuddyAllocator_malloc(&alloc, 100000);
  BuddyAllocator_free(&alloc, p1);
  BuddyAllocator_free(&alloc, p2);
  BuddyAllocator_free(&alloc, p3);




}
