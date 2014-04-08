#include "flash_queue.h"
#include "memory.h"

void flash_queue_create( flash_queue* queue,
                         uint8 start_subsection,
                         uint8 element_size,
                         uint8 subsection_count )
{
  queue->first_subsection = start_subsection+1;
  queue->last_subsection = start_subsection + subsection_count;

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
  fb_write( &queue->counters_block, &queue->counters );
}
void flash_queue_reset( flash_queue* queue )
{
  queue->counters.start = queue->counters.end = MEMORY_SUBSECTION_ADDR( queue->first_subsection );
  save_queue_counters( queue );
}

// TODO: Error reporting and success/failure indications.
void flash_queue_queue( flash_queue* queue, const void* data )
{
  if ( MEMORY_SUBSECTION_OFFSET( queue->counters.end ) <= queue->elem_size )
  {
    if ( MEMORY_ADDR_SUBSECTION( queue->counters.end ) > queue->last_subsection )
    {
      queue->counters.end = MEMORY_SUBSECTION_ADDR( queue->first_subsection ); //TODO: Log that we lost some data
    }
    mem_clear_subsection( queue->counters.end );
  }

  mem_write( queue->counters.end, data, queue->elem_size );
  queue->counters.end += queue->elem_size;

  save_queue_counters( queue );
}

void wrap_start_pointer( flash_queue* queue )
{
  if ( MEMORY_ADDR_SUBSECTION( queue->counters.start ) > queue->last_subsection )
  {
    queue->counters.start = MEMORY_SUBSECTION_ADDR( queue->first_subsection );
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

static uint64 batchdequeue_impl( flash_queue* queue, void** data, uint64 length )
{
  uint64 last_address = MEMORY_SUBSECTION_ADDR( queue->last_subsection + 1 );
  if ( queue->counters.start + length > last_address )
  {
    uint64 overflow = queue->counters.start + length - last_address;
    uint64 rem = length - overflow;
    uint64 count = 0;
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
uint64 flash_queue_batchdequeue( flash_queue* queue, void* data, uint64 count ) {
  if ( flash_queue_empty( queue ) ) {
    return 0;
  }
  uint64 queue_count = flash_queue_count( queue );
  if ( count > queue_count ) {
    count = queue_count;
  }
  uint64 l = queue->elem_size * count;
  batchdequeue_impl( queue, &data, l );
  save_queue_counters( queue );
  return count;
}

uint64 flash_queue_count( const flash_queue* queue ) {
  if ( queue->counters.end >= queue->counters.start ) {
    return (queue->counters.end - queue->counters.start)/queue->elem_size;
  } else {
    uint64 size = MEMORY_SUBSECTION_ADDR(queue->last_subsection+1) - MEMORY_SUBSECTION_ADDR(queue->first_subsection);
    return ( size - (queue->counters.start - queue->counters.end))/queue->elem_size;
  }
}

bool flash_queue_empty( const flash_queue* queue ) {
  return (queue->counters.start == queue->counters.end);
}