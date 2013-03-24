#include "flash_queue.h"
#include "memory.h"

void flash_queue_create( flash_queue* queue,
                         unsigned int start_address,
                         unsigned int element_size,
                         unsigned long length )
{
  queue->start_address = start_address;
  queue->elem_size = element_size;
  queue->size = length * element_size;
  queue->start = queue->end = 0L;
}

static inline void wrap_queue_pointers( flash_queue* queue ) {
  if ( queue->start >= queue->start_address + queue->size ) {
    queue->start = queue->start_address;
  }
  if ( queue->end >= queue->start_address + queue->size ) {
    queue->end = queue->start_address;
  }
}

// TODO: Error reporting and success/failure indications.

void flash_queue_queue( flash_queue* queue, const void* data ) {
  mem_write( queue->end, data, queue->elem_size );
  if (queue->end == queue->start) {
    queue->start += queue->elem_size;
    queue->end = queue->start;
  } else {
    queue->end += queue->elem_size;
  }
  wrap_queue_pointers( queue );
}
void flash_queue_dequeue( flash_queue* queue, void* data ) {
  if ( flash_queue_empty( queue ) ) {
    return;
  }
  mem_read( queue->start, data, queue->elem_size );
  queue->start += queue->elem_size;
  wrap_queue_pointers( queue );
}

void batchqueue_impl( flash_queue* queue, const void** data, unsigned long length )
{
  if ( queue->end + length > queue->start_address + queue->size ) {
    unsigned long overflow = (queue->end + length) - (queue->start_address + queue->size);
    unsigned long rem = length - overflow;
    batchqueue_impl( queue, data, rem);
    *data += rem;
    batchqueue_impl( queue, data, overflow);
    return;
  }
  mem_write( queue->end, *data, length );
  if (queue->end == queue->start) {
    queue->start += queue->elem_size;
    queue->end = queue->start;
  } else {
    queue->end += queue->elem_size;
  }
  wrap_queue_pointers( queue );
  *data += length;
}
void flash_queue_batchqueue( flash_queue* queue, const void* data, unsigned int count ) {
  unsigned long l = queue->elem_size * count;
  while ( l > 0 ) {
    if ( l >= 256 ) {
      batchqueue_impl( queue, &data, 256 );
      l -= 256;
    } else {
      batchqueue_impl( queue, &data, l );
    }
  }
}

void batchdequeue_impl( flash_queue* queue, void** data, unsigned long length ) {
  if ( queue->start + length > queue->start_address + queue->size ) {
    unsigned long overflow = (queue->start + length) - (queue->start_address + queue->size);
    unsigned long rem = length - overflow;
    batchdequeue_impl( queue, data, rem);
    *data += rem;
    batchdequeue_impl( queue, data, overflow);
    return;
  }
  mem_read( queue->start, *data, length );
  queue->start += length;
  wrap_queue_pointers( queue );
  *data += length;
}
void flash_queue_batchdequeue( flash_queue* queue, void* data, unsigned int count ) {
  if ( flash_queue_empty( queue ) || flash_queue_count( queue ) > count ) {
    return;
  }
  unsigned long l = queue->elem_size * count;
  batchdequeue_impl( queue, &data, l );
}

unsigned long flash_queue_count( const flash_queue* queue ) {
  if ( queue->end > queue->start ) {
    return (queue->end - queue->start)/queue->elem_size;
  } else {
    return (queue->size - (queue->start - queue->end))/queue->elem_size;
  }
}

bool flash_queue_empty( const flash_queue* queue ) {
  return (queue->start == queue->end);
}
/*bool flash_queue_full( const flash_queue* queue ) {
  return (queue->end == queue->start_address + queue->size);
}*/