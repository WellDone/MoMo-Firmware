#include "bus_slave.h"
#include "bootloader.h"
#include "flash_memory.h"

uint8 exec_prepare_reflash()
{
	set_firmware_id(plist_get_int8(1));	//get_int8 returns the low word of the second 16 bit integer param
	prepare_reflash(plist_get_int8(0));
	
	return 0x00;
}

uint8 exec_reset()
{
	status.dirty_reset = 0;
	#asm
	reset 
	#endasm

	return 0x00;
}

uint8 exec_verify()
{
	plist_set_int8(0,1, 0);
	plist_set_int8(0,0, verify_application());

	bus_slave_returndata(2);
	return 0x00;
}
