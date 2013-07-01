#include "bus.h"

void bus_init()
{
	mib_firstfree = 0;
	mib_state.num_reads = 0;
	mib_state.slave_state = kMIBIdleState;
	mib_state.master_state = kMIBIdleState;
	
	i2c_enable(0x09);
}