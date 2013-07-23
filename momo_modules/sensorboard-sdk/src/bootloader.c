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

//The first 32 bytes of RAM in bank0 are used for reflashing.
//they can also be used by the application since once we initiate a reflash
//cycle, that application is gone.

uint8 app_buffer[32] @ 0x20;
uint8 boot_source @ 0x40;
uint8 boot_count  @ 0x41;
unsigned int boot_address @ 0x42;

void prepare_reflash(uint8 source)
{
	set_flash_word(14, source, kRetlwHighWord);
	set_flash_word(15, kReflashMagicNumber, kRetlwHighWord);
	flash_erase_row(kNumFlashRows-1);
	flash_write_row(kNumFlashRows-1); //Write the source and magic number into the memory
}

void enter_bootloader()
{
	boot_source = check_bootloader();

	//If boot source is 0 the magic was wrong, we shouldn't bootload
	if (boot_source == 0)
		return;

	for (boot_count = kFirstApplicationRow; boot_count < kNumFlashRows; ++ boot_count)
	{
		//TODO: replace this with a call to the actual endpoint
		bus_master_prepare(0x01, 0x00, plist_no_buffer(1));
    	plist_set_int16(0,5);
    	bus_master_rpc_sync(0x15);

		flash_erase_row(boot_count);
		flash_write_row(boot_count);
	}
}