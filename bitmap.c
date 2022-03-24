#include <assert.h>
#include <stdio.h>
#include "bitmap.h"

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
  return bits/8 + ((bits%8)!=0); // prendo il numero di bytes (per eccesso) per contenere tutti i bit
}

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, uint8_t *buffer){
  bit_map->buffer=buffer;
  bit_map->num_bits=num_bits;
  bit_map->buffer_size=BitMap_getBytes(bit_map->num_bits);
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_num, int status){
  // get byte
  //printf("%d bitn\n",bit_num );
  int byte_num=bit_num>>3;
  //printf("%d byte_num\n",byte_num );
  assert(byte_num<bit_map->buffer_size);

  int bit_in_byte=bit_num&0x07;
  if (status) {
    bit_map->buffer[byte_num] |= (1<<bit_in_byte);
  } else {
    bit_map->buffer[byte_num] &= ~(1<<bit_in_byte);
  }
  /*if (byte_num==0) {
    printf("%d b in b\n",bit_in_byte );
    printf("%d bufbyte\n", bit_map->buffer[byte_num]);  }*/
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num){
  int byte_num=bit_num>>3;
  assert(byte_num < bit_map->buffer_size);
  int bit_in_byte=bit_num&0x07;
  /*printf("%d buffbyte\n",bit_map->buffer[byte_num] );
  printf("%d bitbyte\n",1<<bit_in_byte );
  printf("%d andbit\n",bit_map->buffer[byte_num] & (1<<bit_in_byte) );*/
  return (bit_map->buffer[byte_num] & (1<<bit_in_byte))!=0;
}
