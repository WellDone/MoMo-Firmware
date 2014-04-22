#include "sensor_event_log.h"
#include "flash_queue.h"
#include "ringbuffer.h"
#include "task_manager.h"

#define EVENT_BUFFER_SIZE 32

flash_queue event_log;
static ringbuffer event_buffer;
static sensor_event event_buffer_data[EVENT_BUFFER_SIZE];

void init_sensor_event_log( uint8 start_subsection, uint8 subsection_count )
{
  ringbuffer_create( &event_buffer, event_buffer_data, sizeof( sensor_event ), EVENT_BUFFER_SIZE );
  flash_queue_create( &event_log, start_subsection,
                      sizeof(sensor_event), 
                      subsection_count );
}

static void flush_one_event()
{
  if ( ringbuffer_empty( &event_buffer ) )
    return;
  sensor_event event;
  ringbuffer_pop( &event_buffer, &event );
  flash_queue_queue( &event_log, &event );
}

bool log_sensor_event( uint8 module, SensorMetadata metadata, const rtcc_datetime* datetime, uint32 *value )
{
  sensor_event event;

  event.module = module;
  event.metadata = metadata;

  rtcc_create_timestamp( datetime, &(event.timestamp) );
  event.value = *value;

  ringbuffer_push( &event_buffer, &event );
  taskloop_add( flush_one_event );

  return true;
}

uint32 read_sensor_events( sensor_event* events, uint32 max ) {
  return flash_queue_batchdequeue( &event_log, events, max );
}

void requeue_sensor_events( uint32 count ) {
  flash_queue_requeue( &event_log, count );
}

bool sensor_event_log_empty() {
  return flash_queue_empty( &event_log );
}

uint32 sensor_event_log_count() {
  return flash_queue_count( &event_log );
}

void sensor_event_log_clear() {
  flash_queue_reset( &event_log );
}