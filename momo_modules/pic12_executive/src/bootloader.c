//bootloader.c
/*
 * Code for the pic12 to flash itself over i2c
 */

/*
 * 
 *
 */

#include <xc.h>
#include "bootloader.h"
#include "constants.h"
#include "bus_master.h"
#include "port.h"
#include <string.h>
#include "ioc.h"

//The first N bytes of RAM in bank0 are used for reflashing, where N is 2*the flash row size for the uC.
//they can also be used by the application since once we initiate a reflash
//cycle, that application is gone.

uint8 app_buffer[32] @ 0x20; //ends at 0x60 if 64 bytes, (xc8 chokes if this is larger than 32 bytes, so just fake it.)
uint8 boot_source @ 0x65;
uint8 boot_count  @ 0x66;
uint8 boot_id @ 0x67;
uint8 i @ 0x68;
uint8 tmp @ 0x69;
uint8 invalid_row @ 0x6A;
uint8 offset @ 0x6B;

extern bank1 __persistent MIBExecutiveStatus status;

void enter_bootloader()
{
	boot_source = get_boot_source();
	boot_id = get_firmware_id();

	//Note that we compare to kNumFlashRows -1 in case there are 256 rows, which doesn't fit
	//in an 8-bit unsigned value.  We write this as a do while loop so that we don't overflow
	//boot_count when kNumFlashRows == 256.
	
	boot_count = kFirstApplicationRow-1;

	//Let everyone know that we are reflashing
	ioc_disable(); //make sure we don't reset ourselves when we take the bus down

	LATCH(ALARM) = 0;
	PIN_DIR(ALARM, OUTPUT);

	do
	{
		++boot_count;

    	invalid_row = 1;

    	for (offset = 0; offset < kBootloaderBufferSize; offset += kMIBRequestSize)
    		get_half_row(); //offset is passed through global

		flash_erase_row(boot_count);
		//If there was a problem receiving the row, don't program it.  This is usually b/c
		//the row is not defined in the application hex file, so it should be all 1's (unprogrammed)
		if (!invalid_row)
			flash_write_row(boot_count);

	} while (boot_count != (kNumFlashRows-1));

	//Make sure we can still reset ourselves upon receipt of a reset pulse
	PIN_DIR(ALARM, INPUT);
	ioc_enable(ALARMPORT); 
}