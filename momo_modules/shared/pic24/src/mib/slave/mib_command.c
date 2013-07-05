//mib_command.c 

#include "mib_command.h"
#include "bus_slave.h"
#include <string.h>

//Include command map
#include "commands.h"

extern unsigned char 	mib_buffer[kBusMaxMessageSize];
extern void loadparams(uint8 spec); //ASM function to create the parameters in the mib buffer

uint8 find_handler(unsigned char feature, unsigned char cmd)
{
	uint8 i, num_cmds;
	uint8 found_feat = kNumFeatures;

	for (i=0; i<kNumFeatures; ++i)
	{
		if (features[i] == feature)
		{
			found_feat = i;
			break;
		}
	}

	if (found_feat == kNumFeatures)
		return kInvalidMIBHandler;

	num_cmds = commands[found_feat+1] - commands[found_feat];

	if (cmd >= num_cmds)
		return kInvalidMIBHandler;

	return commands[found_feat] + cmd;
}

uint8 build_params(uint8 handler_index)
{
	loadparams(param_specs[handler_index]);

	return (param_specs[handler_index] & 0b111); //return the count
}