//mib_hal.c
/*
 * pic12 specific versions of the required functions in the MIB hardware abstraction layer.
 * some are implemented in assembly and stored in mib_hal.asm
 */

//external assembly functions for getting application code rpc information (defined in mib_hal.asm)

#include "bus.h"
#include "mib_hal.h"
#include "executive.h"
#include "bootloader.h"
#include <xc.h>

extern bank1 __persistent MIBExecutiveStatus status;

uint8 get_num_features();
uint8 get_feature(uint8 feature);
uint8 get_command(uint8 command);
uint8 get_magic();

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