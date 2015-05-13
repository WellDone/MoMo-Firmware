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
#include "executive_state.h"
#include "bus_master.h"
#include "port.h"
#include <string.h>
#include "ioc.h"

extern uint8 app_buffer[32];
extern uint8 boot_source;
extern uint8 boot_count;
extern uint8 boot_id;
extern uint8 invalid_row;
extern uint8 offset;

/*void enter_bootloader()
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
}*/