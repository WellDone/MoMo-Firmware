#include "sensor_manager.h"
#include "sensor_event_log.h"

typedef uint8 SensorStream;

uint8 add_sensor_stream( uint8 mib_address )
{
	return 0; //TODO
}

void push_sensor_value( uint8 stream_id, uint8 meta, uint64 *value )
{
	rtcc_datetime now;
	rtcc_get_time( &now );

	log_sensor_event( stream_id, meta, &now, value );
}

void schedule_sensor_polling( AlarmRepeatTime interval, uint8 mib_address, uint8 callback_feature, uint8 callback_command )
{

}