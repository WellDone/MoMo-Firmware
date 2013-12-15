//mib_hal.c
/*
 * pic12 specific versions of the required functions in the MIB hardware abstraction layer.
 * some are implemented in assembly and stored in mib_hal.asm
 */

//external assembly functions for getting application code rpc information (defined in mib_hal.asm)

#include "bus.h"
#include "mib_hal.h"
#include "executive.h"
#include <xc.h>


uint8 get_num_features();
uint8 get_feature(uint8 feature);
uint8 get_command(uint8 command);
uint8 get_magic();

/*uint8 find_handler(void)
{
	uint8 found_feat;
	uint8 cmd = mib_state.bus_command.command;

	//Executive features are handled locally and not passed on to the application module
	if (mib_state.bus_command.feature == kMIBExecutiveFeature)
	{
		if (cmd < kNumExecutiveCommands)
			return cmd;
		else
			return kInvalidMIBIndex;
	}

	//Make sure there's valid application code loaded and it has a proper mib callback table
	if (get_magic() != kMIBMagicNumber)
		return kInvalidMIBIndex;

	for (found_feat=0; found_feat<get_num_features(); ++found_feat)
	{
		if (get_feature(found_feat) == mib_state.bus_command.feature)
			break;
	}

	if (found_feat == get_num_features())
		return kInvalidMIBIndex;

	cmd += get_command(found_feat); //commands[found_feat] is the offset in the list for this command

	if (cmd >= get_command(found_feat+1))
		return kInvalidMIBIndex;

	return cmd;
}*/

void bus_init(uint8 address)
{
	i2c_enable(address);

	mib_state.num_reads = 0;
	mib_state.slave_state = kMIBIdleState;
	mib_state.master_state = kMIBIdleState;	
}

uint8 bus_is_idle()
{
	//bus is idle if no start and no stop conditions have been detected and SCL and SDA are both high
	if ((!SSPSTATbits.P) && (!SSPSTATbits.S))
	{
		if (SDAPIN && SCLPIN)
			return 1;
	}

	//If a stop has been asserted with no starts, the bus is idle
	if (SSPSTATbits.P && (!SSPSTATbits.S))
		return 1;

	//Otherwise it is not idle
	return 0;
}