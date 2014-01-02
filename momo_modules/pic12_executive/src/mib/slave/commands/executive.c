#include "bus_slave.h"
#include "bootloader.h"
#include "flash_memory.h"

void exec_prepare_reflash()
{
	set_firmware_id(plist_get_int8(2));
	prepare_reflash(plist_get_int8(0));
	bus_slave_setreturn(pack_return_status(0,0));
}

void exec_reset()
{
	#asm
	reset 
	#endasm
}

void exec_verify()
{
	plist_set_int8(0,1, 0);
	plist_set_int8(0,0, verify_application());

	bus_slave_setreturn(pack_return_status(0,2));
}
