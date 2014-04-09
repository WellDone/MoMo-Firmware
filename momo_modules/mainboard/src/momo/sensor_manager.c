#include "sensor_manager.h"
#include "sensor_event_log.h"

void push_sensor_value( uint8 stream_id, uint8 meta, uint64 *value )
{
	rtcc_datetime now;
	rtcc_get_time( &now );

	log_sensor_event( stream_id, (SensorMetadata)meta, &now, value );
}