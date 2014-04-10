#include "sensor_mib_feature.h"
#include "sensor_manager.h"
#include "sensor_event_log.h"

#include "bus_slave.h"
#include "mib_definitions.h"
#include "mib_feature_definition.h"

static void log_sensor_event_mib(void)
{
	uint8 mib_addr = plist_get_int8(0); 
	uint8 metadata = plist_get_int8(1);
	uint32 *value = (uint32*)plist_get_buffer(2);
	
	//TODO: Keep in-memory queue, lazily write to flash
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

DEFINE_MIB_FEATURE_COMMANDS(sensor) {
	{ 0x00, log_sensor_event_mib, plist_spec(2, true) },
	{ 0x10, read_sensor_event_mib, plist_spec_empty() }
};
DEFINE_MIB_FEATURE(sensor);