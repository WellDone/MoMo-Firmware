#include "bus_master.h"
#include <string.h>
#include "mib_state.h"
#include "bootloader.h"

void bus_master_begin_rpc()
{
	//Wait until the bus is idle to make sure that we don't trample
	//a currently happening slave call
	while (!bus_is_idle())
		;

	//Take control of the bus from the slave so that we can use the
	//shared buffers for master storage
	i2c_master_enable();
}