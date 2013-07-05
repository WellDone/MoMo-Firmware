//mib_command.c 

#include "mib_command.h"
#include "bus_slave.h"
#include <string.h>

//device must define a command map 
#include "command_map.h"

extern void loadparams(uint8 spec); //ASM function to create the parameters in the mib buffer

unsigned char find_handler(unsigned char feature, unsigned char cmd)
{
	uint8 found_feat;

	//Make sure there's valid application code loaded and it has a proper mib callback table
	if (get_magic() != kMIBMagicNumber)
		return kInvalidMIBIndex;

	for (found_feat=0; found_feat<get_num_features(); ++found_feat)
	{
		if (get_feature(found_feat) == feature)
			break;
	}

	if (found_feat == get_num_features())
		return kInvalidMIBIndex;

	cmd += get_command(found_feat); //commands[found_feat] is the offset in the list for this command

	if (cmd >= get_command(found_feat+1))
		return kInvalidMIBIndex;

	return cmd;
}

uint8 build_params(uint8 handler_index)
{
	loadparams(get_spec(handler_index));

	return (get_spec(handler_index) & 0b111); //return the count
}