//bootloader.c
/*
 * Code for the pic12 to flash itself over i2c
 */

/*
 * 
 *
 */

#include "bootloader.h"
#include "bus_master.h"
#include <string.h>

//The first 32 bytes of RAM in bank0 are used for reflashing.
//they can also be used by the application since once we initiate a reflash
//cycle, that application is gone.

uint8 app_buffer[32] @ 0x20;
uint8 boot_source @ 0x40;
uint8 boot_count  @ 0x41;
unsigned int boot_address @ 0x42;
uint8 boot_id @ 0x44;
uint8 i @ 0x45;
uint8 tmp @ 0x46;


void set_firmware_id(uint8 bucket)
{
	set_flash_word(13, bucket, kRetlwHighWord);
}

void prepare_reflash(uint8 source)
{
	set_flash_word(14, source, kRetlwHighWord);
	set_flash_word(15, kReflashMagicNumber, kRetlwHighWord);
	flash_erase_row(kNumFlashRows-1);
	flash_write_row(kNumFlashRows-1); //Write the source and magic number into the memory
}

void check_enter_bootloader()
{
	boot_source = check_bootloader();

	//If boot source is 0 the magic was wrong, we're not in bootloader mode
	if (boot_source == 0)
		return;

	boot_id = get_firmware_id();

	for (boot_count = kFirstApplicationRow; boot_count < kNumFlashRows; ++boot_count)
	{
		//Multiply by 16 to convert from rows to words then by 2 to convert to bytes
		boot_address = ((unsigned int)(boot_count)) << 5;
		
		bus_master_prepare(0x07, 0x04, plist_no_buffer(2));
    	plist_set_int16(0,boot_id);
    	plist_set_int16(1,boot_address);

    	bus_master_rpc_sync(boot_source);
    	memcpy(app_buffer, mib_buffer, 20);

    	//2nd half of the row
    	boot_address += 8;
    	bus_master_prepare(0x07, 0x04, plist_no_buffer(2));
    	plist_set_int16(0,boot_id);
    	plist_set_int16(1,boot_address);

    	bus_master_rpc_sync(boot_source);
    	memcpy(app_buffer+20, mib_buffer, 12);

    	//Data from the pic24 comes in big endian format, we expect little endian
    	//so flip the buffer.
    	flip_bootbuffer_endianness();

		flash_erase_row(boot_count);
		flash_write_row(boot_count);
	}
}

void flip_bootbuffer_endianness()
{
	for (i=0; i<32; i+=2)
	{
		tmp = app_buffer[i];
		app_buffer[i] = app_buffer[i+1];
		app_buffer[i+1] = tmp;
	}
}