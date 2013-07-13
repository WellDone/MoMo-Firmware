//mib_command.c 

#include "bus.h"
#include "bus_master.h"
#include "bus_slave.h"
#include <string.h>

//Include command map
#include "commands.h"

uint8 find_handler(void)
{
	uint8 i, num_cmds;
	uint8 cmd = mib_state.bus_command.command;
	uint8 found_feat = kNumFeatures;

	for (i=0; i<kNumFeatures; ++i)
	{
		if (features[i] == mib_state.bus_command.feature)
		{
			found_feat = i;
			break;
		}
	}

	if (found_feat == kNumFeatures)
		return kInvalidMIBIndex;

	num_cmds = commands[found_feat+1] - commands[found_feat];

	if (cmd >= num_cmds)
		return kInvalidMIBIndex;

	return commands[found_feat] + cmd;
}

void  call_handler(uint8 handler_index)
{
	handlers[handler_index]();
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

void bus_init()
{
	I2CConfig config;

	config.address = kControllerPICAddress;
	config.priority = 0b010;
	config.callback = bus_master_callback;
	config.slave_callback = bus_slave_callback;

	i2c_init_flags(&config);
	i2c_set_flag(&config, kEnableGeneralCallFlag);
	i2c_set_flag(&config, kEnableSoftwareClockStretchingFlag);

	i2c_configure(&config);
	i2c_enable();
}