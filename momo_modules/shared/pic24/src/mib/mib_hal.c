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

uint8 validate_params(uint8 handler_index)
{
	return 1;
}

uint8 loadparams(uint8 param_spec)
{
	uint8 param_cnt = extract_param_count(param_spec);
	uint8 i;
	uint8 *ptr = mib_buffer;

	for (i=0; i<param_cnt; ++i)
	{
		if (extract_param_type(param_spec, i) == kMIBInt16Type)
		{
			MIBIntParameter *param = (MIBIntParameter*)ptr;
			param->header.type = kMIBInt16Type;
			param->header.len = 2;
			param->value = 0;

			ptr += sizeof(MIBIntParameter);
		}
		else
		{
			MIBBufferParameter *param = (MIBBufferParameter*)ptr;
			param->header.type = kMIBBufferType;
			param->header.len = kBusMaxMessageSize - (ptr-mib_buffer) - 2;
			ptr += sizeof(MIBParameterHeader);
		}
	}

	return (uint8)(ptr-mib_buffer);
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