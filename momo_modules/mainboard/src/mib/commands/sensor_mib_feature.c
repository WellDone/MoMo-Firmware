#include "sensor_mib_feature.h"
#include "sensor_manager.h"
#include "sensor_event_log.h"

#include "bus_slave.h"
#include "mib_definitions.h"
#include "mib_feature_definition.h"

#include "flash_queue.h"

static uint8_t log_sensor_event_mib(uint8_t length)
{
	uint8_t mib_addr = plist_get_int8(0); 
	uint8_t metadata = plist_get_int8(1);
	uint32 value = *(uint32*)plist_get_buffer(2);
	
	push_sensor_value(mib_addr, metadata, value);

	return kNoErrorStatus;
}

static uint8_t read_sensor_event_mib(uint8_t length)
{
	if ( 0 == read_sensor_events( (sensor_event*)plist_get_buffer(0), 1 ) )
	{
		return kCallbackError;
	}
	else
		bus_slave_set_returnbuffer_length(sizeof(sensor_event));

	return kNoErrorStatus;
}

static uint8_t clear_log(uint8_t length)
{
	sensor_event_log_clear();
	return kNoErrorStatus;
}

static uint8_t log_count(uint8_t length)
{
	*((uint32*)plist_get_buffer(0)) = sensor_event_log_count();
	bus_slave_set_returnbuffer_length(sizeof(uint32));

	return kNoErrorStatus;
}

extern flash_queue event_log;
static uint8_t log_debug(uint8_t length)
{
	*((uint32*)plist_get_buffer(0)) = event_log.start_address;
	*((uint32*)plist_get_buffer(2)) = event_log.end_address;
	*((uint32*)plist_get_buffer(4)) = event_log.counters.start;
	*((uint32*)plist_get_buffer(6)) = event_log.counters.end;

	bus_slave_set_returnbuffer_length(4 * sizeof(uint32));
	return kNoErrorStatus;
}

DEFINE_MIB_FEATURE_COMMANDS(sensor) {
	{ 0x00, log_sensor_event_mib},
	{ 0x01, read_sensor_event_mib},
	{ 0x02, log_count},
	{ 0x03, clear_log},
	{ 0x04, log_debug}
};
DEFINE_MIB_FEATURE(sensor);