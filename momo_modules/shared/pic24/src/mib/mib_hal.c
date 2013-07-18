//mib_command.c 

#include "bus.h"
#include "bus_master.h"
#include "bus_slave.h"
#include "mib_feature.h"
#include <string.h>

//Include command map
//#include "commands.h"

extern const feature_map** the_features;
extern unsigned int the_feature_count;

uint8 find_handler(void)
{
	uint8 i, j, num_cmds;

	for (i=0; i<the_feature_count; ++i)
	{
		if (the_features[i]->id == mib_state.bus_command.feature)
		{
			break;
		}
	}

	if (i == the_feature_count)
		return kInvalidMIBIndex;
	mib_state.feature_index = i;

	num_cmds = the_features[i]->command_count;

	for (j=0; j<num_cmds; ++j) {
		if ( the_features[i]->commands[j].id == mib_state.bus_command.command) {
			return j;
		}
	}
	return kInvalidMIBIndex;
}

void  call_handler(uint8 handler_index)
{
	the_features[mib_state.feature_index]->commands[handler_index].handler();
}

void bus_init(uint8 address)
{
	I2CConfig config;

	config.address = address;
	config.priority = 0b010;
	config.callback = bus_master_callback;
	config.slave_callback = bus_slave_callback;

	i2c_init_flags(&config);
	i2c_set_flag(&config, kEnableGeneralCallFlag);
	i2c_set_flag(&config, kEnableSoftwareClockStretchingFlag);

	mib_state.num_reads = 0;
	mib_state.slave_state = kMIBIdleState;
	mib_state.master_state = kMIBIdleState;

	i2c_configure(&config);
	i2c_enable();
}

uint8 get_param_spec(uint8 handler_index)
{
	the_features[mib_state.feature_index]->commands[handler_index].params;
}

uint8 plist_int_count(uint8 plist)
{
	return (plist & 0b01100000) >> 5;
}

uint8 plist_param_length(uint8 plist)
{
	return ((plist & 0b01100000) >> 4) + (plist & 0b00011111);
}