#include "sensor_event_log.h"
#include "flash_queue.h"

static flash_queue event_log;

void init_sensor_event_log( unsigned int start_address, unsigned int max_size )
{
  flash_queue_create( &event_log, start_address, sizeof(sensor_event), max_size / sizeof(sensor_event) );
}

void log_sensor_event( sensor_type type, const rtcc_datetime* datetime, unsigned int value )
{
  sensor_event event;
  event.starttime.date.year = datetime->year;
  event.starttime.date.month = datetime->month;
  event.starttime.date.day = datetime->day;
  event.starttime.hour = datetime->hours;
  event.starttime.minute = datetime->minutes;

  event.type = type & 0xFF;
  event.value = value;

  flash_queue_queue( &event_log, &event );
}

unsigned int read_sensor_events( sensor_event* events, unsigned int max ) {
  return flash_queue_batchdequeue( &event_log, events, max );
}

bool sensor_event_log_empty() {
  return flash_queue_empty( &event_log );
}