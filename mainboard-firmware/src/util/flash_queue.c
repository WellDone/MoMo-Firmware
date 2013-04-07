#include "flash_queue.h"
#include "memory.h"

void flash_queue_create( flash_queue* queue,
                         unsigned long start_address,
                         unsigned int element_size,
                         unsigned long length )
{
  unsigned long subsection_addr, end_address;
  queue->start_address = start_address&~MEMORY_SUBSECTION_MASK; //Align to subsection boundary
  queue->elem_size = element_size;
  queue->size = ((length * element_size)&MEMORY_SUBSECTION_MASK)+MEMORY_SUBSECTION_SIZE;
  end_address = queue->start_address + queue->size;
  for ( subsection_addr = queue->start_address;
        subsection_addr < end_address;
        subsection_addr += MEMORY_SUBSECTION_SIZE )
  {
    mem_clear_subsection( subsection_addr );
  }
  queue->start = queue->end = queue->first_dirty_address = queue->start_address;
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
  queue->end += queue->elem_size;
  if ( queue->end != queue->start && queue->end == queue->first_dirty_address ) {
    mem_clear_subsection( queue->first_dirty_address );
    queue->first_dirty_address += MEMORY_SUBSECTION_SIZE;
    if ( queue->start < queue->first_dirty_address ) {
      queue->start = queue->first_dirty_address;
    }
  }
  wrap_queue_pointers( queue );
}
bool flash_queue_dequeue( flash_queue* queue, void* data ) {
  if ( flash_queue_empty( queue ) ) {
    return false;
  }
  mem_read( queue->start, data, queue->elem_size );
  queue->start += queue->elem_size;
  wrap_queue_pointers( queue );
  return true;
}

unsigned int batchdequeue_impl( flash_queue* queue, void** data, unsigned int length ) {
  if ( queue->start + length > queue->start_address + queue->size ) {
    unsigned int overflow = (queue->start + length) - (queue->start_address + queue->size);
    unsigned int rem = length - overflow;
    unsigned int count = 0;
    count += batchdequeue_impl( queue, data, rem);
    *data += rem;
    count += batchdequeue_impl( queue, data, overflow);
    return count;
  }
  mem_read( queue->start, *data, length );
  queue->start += length;
  wrap_queue_pointers( queue );
  *data += length;
  return length;
}
unsigned int flash_queue_batchdequeue( flash_queue* queue, void* data, unsigned int count ) {
  if ( flash_queue_empty( queue ) ) {
    return 0;
  }
  unsigned int queue_count = flash_queue_count( queue );
  if ( count > queue_count ) {
    count = queue_count;
  }
  unsigned int l = queue->elem_size * count;
  batchdequeue_impl( queue, &data, l );
  return count;
}

unsigned long flash_queue_count( const flash_queue* queue ) {
  if ( queue->end >= queue->start ) {
    return (queue->end - queue->start)/queue->elem_size;
  } else {
    return (queue->size - (queue->start - queue->end))/queue->elem_size;
  }
}

bool flash_queue_empty( const flash_queue* queue ) {
  return (queue->start == queue->end);
}