#include "bus_slave.h"
#include "bootloader.h"

void exec_prepare_reflash()
{
	set_firmware_id(plist_get_int16(1));
	prepare_reflash(plist_get_int16(0));
	bus_slave_setreturn(pack_return_status(0,0));
}

void exec_reset()
{
	#asm
	reset 
	#endasm
}
