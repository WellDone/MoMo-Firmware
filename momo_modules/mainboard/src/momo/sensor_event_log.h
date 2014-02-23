#ifndef __sensor_event_log_h__
#define __sensor_event_log_h__

#include "rtcc.h"

typedef enum {
  momo_pulse_counter=0
} sensor_type;

typedef struct {
  rtcc_date date; //3
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
} sensor_event_timestamp;

typedef struct {
  unsigned char type; //1
  sensor_event_timestamp starttime; //5
  unsigned long value; //4
} sensor_event;

void init_sensor_event_log( unsigned long start_address, unsigned long max_size );
void log_sensor_event( sensor_type type, const rtcc_datetime* timestamp, unsigned long value );
unsigned int read_sensor_events( sensor_event* events, unsigned int max );
bool sensor_event_log_empty();
unsigned long sensor_event_log_count();

#endif