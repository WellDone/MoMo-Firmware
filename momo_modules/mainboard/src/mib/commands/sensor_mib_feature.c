#include "sensor_mib_feature.h"
#include "sensor_manager.h"
#include "sensor_event_log.h"

#include "bus_slave.h"
#include "mib_definitions.h"
#include "mib_feature_definition.h"

#include "flash_queue.h"

static void log_sensor_event_mib(void)
{
	uint8 mib_addr = plist_get_int8(0); 
	uint8 metadata = plist_get_int8(1);
	uint32 *value = (uint32*)plist_get_buffer(2);
	
	push_sensor_value( mib_addr, metadata, value );

	bus_slave_setreturn(pack_return_status(kNoMIBError, 0));
}

static void read_sensor_event_mib(void)
{
	if ( 0 == read_sensor_events( (sensor_event*)plist_get_buffer(0), 1 ) )
	{
		bus_slave_setreturn( pack_return_status( kCallbackError, 0 ) );
	}
	else
	{	
		bus_slave_setreturn( pack_return_status( kNoMIBError, sizeof(sensor_event) ) );
	}
}

static void clear_log(void)
{
	sensor_event_log_clear();
	bus_slave_setreturn( pack_return_status( kNoMIBError, 0 ) );
}

static void log_count(void)
{
	*((uint32*)plist_get_buffer(0)) = sensor_event_log_count();
	bus_slave_setreturn( pack_return_status( kNoMIBError, sizeof( uint32 ) ) );
}

extern flash_queue event_log;
static void log_debug(void)
{
	*((uint32*)plist_get_buffer(0)) = event_log.start_address;
	*((uint32*)plist_get_buffer(2)) = event_log.end_address;
	*((uint32*)plist_get_buffer(4)) = event_log.counters.start;
	*((uint32*)plist_get_buffer(6)) = event_log.counters.end;
	bus_slave_setreturn( pack_return_status( kNoMIBError, 4 * sizeof( uint32 ) ) );	
}

DEFINE_MIB_FEATURE_COMMANDS(sensor) {
	{ 0x00, log_sensor_event_mib, plist_spec(2, true) },
	{ 0x01, read_sensor_event_mib, plist_spec_empty() },
	{ 0x02, log_count, plist_spec_empty() },
	{ 0x03, clear_log, plist_spec_empty() },
	{ 0x04, log_debug, plist_spec_empty() }
};
DEFINE_MIB_FEATURE(sensor);