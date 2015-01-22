#ifndef __sensor_event_log_h__
#define __sensor_event_log_h__

#include "rtcc.h"

typedef union
{
	struct
	{
		uint8 error:1;
		uint8 stream_id:3; // MAX: 8 sensor streams per module
		uint8 aux:4; // Up to the sensor module, currently unused
	};
	uint8 metadata;
} SensorMetadata;

typedef struct {
  uint8          module;      //1
  SensorMetadata metadata;    //1
  rtcc_timestamp timestamp;   //4
  uint16		 reserved;	  //2
  uint32         value;       //4
} sensor_event;             //= 12

void init_sensor_event_log( uint8 start_subsector, uint8 num_subsectors );
bool log_sensor_event( uint8 module, SensorMetadata metadata, const rtcc_datetime* timestamp, uint32 value );
uint32 read_sensor_events( sensor_event* events, uint32 max );
void requeue_sensor_events( uint32 count );
bool sensor_event_log_empty();
void sensor_event_log_clear();
uint32 sensor_event_log_count();

#endif