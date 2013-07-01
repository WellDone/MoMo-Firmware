//mib_command.c 

#include "mib_command.h"
#include "bus_slave.h"
#include <string.h>

//device must define a command map 
#include "commands.h"

unsigned char find_handler(unsigned char feature, unsigned char cmd)
{
	uint8 found_feat;

	for (found_feat=0; found_feat<kNumFeatures; ++found_feat)
	{
		if (features[found_feat] == feature)
			break;
	}

	if (found_feat == kNumFeatures)
		return 255;

	cmd += commands[found_feat]; //commands[found_feat] is the offset in the list for this command

	if (cmd >= commands[found_feat+1])
		return 255;

	return cmd;
}

mib_callback get_handler(unsigned char index)
{
	if (index == 255)
		return NULL;

	return handlers[index];
}

MIBParamList *	build_params(unsigned char handler_index)
{
	unsigned char spec 			= param_specs[handler_index];
	unsigned char num_params 	= extract_param_count(spec);
	MIBParamList *list 			= bus_allocate_param_list(num_params);
	unsigned char i;
	
	for (i=0; i<num_params; ++i)
	{
		unsigned char type = extract_param_type(spec, i);

		if (type == kMIBInt16Type)
			list->params[i] = (MIBParameterHeader*)bus_allocate_int_param();
		else if(type == kMIBBufferType && i == (num_params-1))
		{
			//If we have 1 buffer as the last parameter, allocate all the remaining space for it from the mib_buffer
			list->params[i] = (MIBParameterHeader*)bus_allocate_buffer_param(0);
		}
		else
			return NULL; //We cannot allocate buffers that are not the last parameter since we don't know how big they should be
	}

	return list;
}