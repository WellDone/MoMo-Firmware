#ifndef __flash_queue_h__
#define __flash_queue_h__

#include "common.h"
#include "flashblock.h"

/*
  Derivitave of ringbuffer.c, implemented as an in-flash ringbuffer.
  Overwrites oldest values when ringbuffer is full.
*/

typedef struct {
	uint32   start; //the first valid item
  uint32   end;   //one past the last valid item
} flash_queue_counters;

typedef struct {
  uint8                elem_size;
  uint32               start_address;
  uint32               end_address;
  bool                 wrapped;

  flash_block_info     counters_block;

  flash_queue_counters counters;
} flash_queue;

typedef struct {
  uint32 location;
  const flash_queue* queue;
} flash_queue_walker;

void flash_queue_create( flash_queue* queue,
                         uint8 start_subsection,
                         uint8 element_size,
                         uint8 subsection_count ); //subsection_count must be >= 2
void flash_queue_reset( flash_queue* queue );

void flash_queue_queue( flash_queue* queue, const void* data );
bool flash_queue_dequeue( flash_queue* queue, void* data );
uint32 flash_queue_batchdequeue( flash_queue* queue, void* data, uint32 count );
void flash_queue_requeue( flash_queue* queue, uint32 count );

bool flash_queue_empty( const flash_queue* queue );
uint32 flash_queue_count( const flash_queue* queue );

flash_queue_walker new_flash_queue_walker( const flash_queue* queue, uint32 offset );
uint8 flash_queue_walk( flash_queue_walker* walker, void* data, uint8 batch_size );

#endif