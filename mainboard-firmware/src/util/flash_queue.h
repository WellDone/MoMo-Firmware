#ifndef __flash_queue_h__
#define __flash_queue_h__

#include "common.h"

/*
  Derivitave of ringbuffer.c, implemented as an in-flash ringbuffer.
  Overwrites oldest values when ringbuffer is full.
*/

typedef struct {
  unsigned int    elem_size;
  unsigned long   start_address;
  unsigned long   first_dirty_address;
  unsigned long   size;

  unsigned long   start; //the first valid item
  unsigned long   end; //one past the last valid item
} flash_queue;

void flash_queue_create( flash_queue* queue, unsigned long start_address, unsigned int element_size, unsigned long length );
void flash_queue_queue( flash_queue* queue, const void* data );
bool flash_queue_dequeue( flash_queue* queue, void* data );
unsigned int flash_queue_batchdequeue( flash_queue* queue, void* data, unsigned int count );

bool flash_queue_empty( const flash_queue* queue );
unsigned long flash_queue_count( const flash_queue* queue );

#endif