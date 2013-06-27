#include "mib_command_parameters.h"
#include "bus.h"

volatile MIBParamList *	build_params(mib_command_handler* handler)
{
	BYTE spec = handler->param_spec;
	unsigned char num_params 	= extract_param_count(spec);
	volatile MIBParamList *list = bus_allocate_param_list(num_params);
	unsigned int i;
	
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