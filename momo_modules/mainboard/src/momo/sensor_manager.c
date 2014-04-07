#include "sensor_manager.h"

typedef uint8 SensorStream;

static SensorStream sensor_streams[MAX_SENSOR_STREAM_COUNT];

void init_sensor_manager()
{
	
}

void register_sensor( uint8 mib_address, uint8 stream_count )
{

}

uint8 add_sensor_stream( uint8 mib_address )
{
	return 0; //TODO
}

void push_sensor_value( uint8 mib_address, uint8 stream_index, uint8 meta, uint64 *value )
{
	uint8 stream_id = stream_index; //TODO: translate to global ID so we're ok if we have more than one sensor module

	rtcc_datetime now;
	rtcc_get_time( &now );

	log_sensor_event( stream_id, meta, &now, value );
}

void schedule_sensor_polling( AlarmRepeatTime interval, uint8 mib_address, uint8 callback_feature, uint8 callback_command )
{

}