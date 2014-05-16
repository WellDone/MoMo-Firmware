#include "bus_slave.h"
#include "mib_definitions.h"
#include "mib_feature_definition.h"
#include "scheduler_mib_feature.h"
#include "scheduler.h"
#include "common_types.h"
#include "bus_master.h"

typedef struct {
	uint8 address;
	uint8 feature;
	uint8 command;

	AlarmRepeatTime frequency;

	ScheduledTask task;
} scheduled_callback;

#define MAX_SCHEDULED_CALLBACKS 16
static scheduled_callback callbacks[MAX_SCHEDULED_CALLBACKS];

static void callback( void* arg )
{
	scheduled_callback *cb = (scheduled_callback*) arg;

	MIBUnified cmd;
	cmd.address = cb->address;
  cmd.bus_command.feature = cb->feature;
  cmd.bus_command.command = cb->command;
  cmd.bus_command.param_spec = plist_empty();
	bus_master_rpc_async( NULL, &cmd );
}

void schedule_callback()
{
	uint8 address = plist_get_int16(0) & 0xFF;
	uint8 feature = plist_get_int16(1) >> 8;
	uint8 command = plist_get_int16(1) & 0xFF;
	uint8 frequency = plist_get_int16(2) & 0xFF;

	uint8 index = 0;
	for ( index = 0; index < MAX_SCHEDULED_CALLBACKS; ++index )
	{
		if ( callbacks[index].address == 0 )
		{
			scheduled_callback* cb = &(callbacks[index]);
			cb->address = address;
			cb->feature = feature;
			cb->command = command;
			cb->frequency = frequency;

			scheduler_schedule_task( callback, cb->frequency, kScheduleForever, &(cb->task), (void*) cb );
			break;
		}
	}
	if ( index == 16 )
		bus_slave_seterror(kCallbackError);
}

void stop_scheduled_callback()
{
	uint8 address = plist_get_int16(0) & 0xFF;
	uint8 feature = plist_get_int16(1) >> 8;
	uint8 command = plist_get_int16(1) & 0xFF;
	uint8 frequency = plist_get_int16(2) & 0xFF;

	uint8 index;
	for ( index = 0; index < MAX_SCHEDULED_CALLBACKS; ++index )
	{
		if ( callbacks[index].address == address &&
			   callbacks[index].feature == feature &&
			   callbacks[index].command == command &&
			   callbacks[index].frequency == frequency )
		{
			scheduler_remove_task( &callbacks[index].task );
			callbacks[index].address = 0;
		}
	}
}

void get_callback_map()
{
	uint32 callback_map = 0;
	uint8 index;
	for ( index = 0; index < MAX_SCHEDULED_CALLBACKS; ++index )
	{
		if ( callbacks[index].address != 0 )
			callback_map |= 0x1 << index;
	}
	bus_slave_return_int16( callback_map );
}

void describe_callback()
{
	uint8 index = plist_get_int8(0);
	if ( index >= MAX_SCHEDULED_CALLBACKS || callbacks[index].address == 0 )
	{
		bus_slave_seterror( kCallbackError );
		return;
	}
	bus_slave_return_buffer( &(callbacks[index]), sizeof(scheduled_callback) );
}

DEFINE_MIB_FEATURE_COMMANDS(scheduler) {
	{0x00, schedule_callback, plist_spec(3,false) },
	{0x01, stop_scheduled_callback, plist_spec(3,false) },
	{0x02, get_callback_map, plist_spec(0,false) },
	{0x03, describe_callback, plist_spec(1,false) }
};
DEFINE_MIB_FEATURE(scheduler);