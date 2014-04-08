#ifndef __sensor_event_log_h__
#define __sensor_event_log_h__

#include "rtcc.h"

typedef struct {
  uint8          stream_id; //1
  uint8          meta;      //1
  rtcc_timestamp timestamp; //6
  uint64         value;     //8
} sensor_event;           //= 16

void init_sensor_event_log( uint8 start_subsector, uint8 num_subsectors );
bool log_sensor_event( uint8 stream_id, uint8 meta, const rtcc_datetime* timestamp, uint64 *value );
uint32 read_sensor_events( sensor_event* events, uint32 max );
bool sensor_event_log_empty();
uint32 sensor_event_log_count();

#endif