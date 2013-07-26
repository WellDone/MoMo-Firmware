#include "bus_slave.h"

void exec_prepare_reflash()
{
	bus_slave_setreturn(pack_return_status(0,0));
}

void exec_reset()
{
	#asm
	reset 
	#endasm
}