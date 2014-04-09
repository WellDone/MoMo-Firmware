#include "sensor_event_log.h"
#include "flash_queue.h"

static flash_queue event_log;

void init_sensor_event_log( uint8 start_subsection, uint8 subsection_count )
{
  flash_queue_create( &event_log, start_subsection,
                      sizeof(sensor_event), 
                      subsection_count );
}

bool log_sensor_event( uint8 module, SensorMetadata metadata, const rtcc_datetime* datetime, uint64 *value )
{
  sensor_event event;

  event.module = module;
  event.metadata = metadata;

  rtcc_create_timestamp( datetime, &event.timestamp );
  event.value = *value;

  flash_queue_queue( &event_log, &event );
  return true;
}

uint32 read_sensor_events( sensor_event* events, uint32 max ) {
  return flash_queue_batchdequeue( &event_log, events, max );
}

bool sensor_event_log_empty() {
  return flash_queue_empty( &event_log );
}

uint32 sensor_event_log_count() {
  return flash_queue_count( &event_log );
}