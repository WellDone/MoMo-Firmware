#include "flash_queue.h"
#include "memory.h"

void flash_queue_create( flash_queue* queue,
                         uint8 start_subsection,
                         uint8 element_size,
                         uint8 subsection_count )
{
  queue->start_address = MEMORY_SUBSECTION_ADDR( start_subsection+1 );
  queue->end_address = MEMORY_SUBSECTION_ADDR( start_subsection + 3 );

  queue->elem_size = element_size;

  fb_init( &queue->counters_block, start_subsection, sizeof( flash_queue_counters ) );
  if ( fb_count( &queue->counters_block ) > 0 )
  {
    fb_read( &queue->counters_block, &queue->counters );
  }
  else
  {
    flash_queue_reset( queue );
  }
}

static inline void save_queue_counters( flash_queue* queue )
{
  //TODO: Batch these writes...?
  fb_write( &queue->counters_block, &queue->counters );
}
void flash_queue_reset( flash_queue* queue )
{
  queue->counters.start = queue->counters.end = queue->start_address;
  queue->wrapped = false;
  save_queue_counters( queue );
}

// TODO: Error reporting and success/failure indications.
void flash_queue_queue( flash_queue* queue, const void* data )
{
  if ( queue->counters.end + queue->elem_size > queue->end_address )
  {
    queue->counters.end = queue->start_address;
    queue->wrapped = true;
  }

  if ( MEMORY_SUBSECTION_OFFSET( queue->counters.end ) < queue->elem_size )
  {
    mem_clear_subsection( queue->counters.end );

    // If `start` is in the subsection we just cleared, bump it to the next one.
    uint32 next_subsection_addr = MEMORY_ADDR_SUBSECTION_ADDR(queue->counters.end) + MEMORY_SUBSECTION_SIZE;
    if ( queue->wrapped &&
         queue->counters.start >= MEMORY_ADDR_SUBSECTION_ADDR(queue->counters.end) &&
         queue->counters.start < next_subsection_addr )
    {
      queue->counters.start = next_subsection_addr; //TODO: Log that we lost some data
    }
  }

  mem_write( queue->counters.end, data, queue->elem_size );
  queue->counters.end += queue->elem_size;

  save_queue_counters( queue );
}

void wrap_start_pointer( flash_queue* queue )
{
  if ( queue->counters.start > queue->end_address )
  {
    queue->counters.start = queue->start_address;
  }
}
bool flash_queue_dequeue( flash_queue* queue, void* data )
{
  if ( flash_queue_empty( queue ) )
    return false;
  mem_read( queue->counters.start, data, queue->elem_size );
  queue->counters.start += queue->elem_size;
  
  wrap_start_pointer( queue );
  save_queue_counters( queue );
  return true;
}

static uint32 batchdequeue_impl( flash_queue* queue, void** data, uint32 length )
{
  if ( queue->counters.start + length > queue->end_address )
  {
    uint32 overflow = queue->counters.start + length - queue->end_address;
    uint32 rem = length - overflow;
    uint32 count = 0;
    count += batchdequeue_impl( queue, data, rem);
    count += batchdequeue_impl( queue, data, overflow);
    return count;
  }
  mem_read( queue->counters.start, *data, length );
  queue->counters.start += length;
  wrap_start_pointer( queue );

  *data += length;
  return length / queue->elem_size;
}
uint32 flash_queue_batchdequeue( flash_queue* queue, void* data, uint32 count ) {
  if ( flash_queue_empty( queue ) ) {
    return 0;
  }
  uint32 queue_count = flash_queue_count( queue );
  if ( count > queue_count ) {
    count = queue_count;
  }
  uint32 l = queue->elem_size * count;
  batchdequeue_impl( queue, &data, l );
  save_queue_counters( queue );
  return count;
}

void flash_queue_requeue( flash_queue* queue, uint32 count ) {
  uint32 new_start = queue->counters.start - ( count * queue->elem_size );
  if ( new_start < queue->counters.end ) //TODO: Possible race condition
    new_start = queue->counters.end;
  else if ( new_start < queue->start_address )
    new_start = queue->start_address;
  queue->counters.start = new_start;
}

bool flash_queue_peek( flash_queue* queue, void* data ) {
  if ( flash_queue_empty( queue ) )
    return false;
  mem_read( queue->counters.start, data, queue->elem_size );
  return true;
}

uint32 flash_queue_count( const flash_queue* queue ) {
  if ( queue->counters.end >= queue->counters.start ) {
    return (queue->counters.end - queue->counters.start)/queue->elem_size;
  } else {
    uint32 size = queue->end_address - queue->start_address;
    return ( size - (queue->counters.start - queue->counters.end))/queue->elem_size;
  }
}

bool flash_queue_empty( const flash_queue* queue ) {
  return (queue->counters.start == queue->counters.end);
}