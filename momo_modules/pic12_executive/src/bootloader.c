//bootloader.c
/*
 * Code for the pic12 to flash itself over i2c
 */

/*
 * 
 *
 */

#include "bootloader.h"
#include "constants.h"
#include "bus_master.h"
#include <string.h>

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

extern __persistent MIBExecutiveStatus status;

void set_firmware_id(uint8 bucket)
{
	set_flash_word(kFlashRowSize-3, bucket, kRetlwHighWord);
}

void prepare_reflash(uint8 source)
{
	status.valid_app = 0;

	set_flash_word(kFlashRowSize-2, source, kRetlwHighWord);
	set_flash_word(kFlashRowSize-1, kReflashMagicNumber, kRetlwHighWord);
	flash_erase_row(kMIBStructRow);
	flash_write_row(kMIBStructRow); //Write the source and magic number into the memory
}

void get_half_row(uint8 offset)
{
	plist_set_int8(0, 0, boot_id);
	plist_set_int8(0, 1, 0);
	plist_set_int8(1, 0, offset);
	bus_master_prepare(0x07, 0x04, plist_no_buffer(2));

	load_boot_address();

	invalid_row |= bus_master_rpc_sync(boot_source);
	copy_mib_to_boot(offset);
}

void enter_bootloader()
{
	boot_source = get_boot_source();
	boot_id = get_firmware_id();

	for (boot_count = kFirstApplicationRow; boot_count < kNumFlashRows; ++boot_count)
	{
    	invalid_row = 0;

    	for (tmp = 0; tmp < kBootloaderBufferSize; tmp += kMIBRequestSize)
    		get_half_row(tmp);

		flash_erase_row(boot_count);
		//If there was a problem receiving the row, don't program it.  This is usually b/c
		//the row is not defined in the application hex file, so it should be all 1's (unprogrammed)
		if (!invalid_row)
		{
			flash_write_row(boot_count);
		}
	}
}