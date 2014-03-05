//bootloader.c

#include "bootloader.h"
#include "memory.h"
#include "constants.h"

void _BOOTLOADER_CODE write_row(unsigned int row, unsigned char *row_buffer)
{
	unsigned int j;

	TBLPAG = 0;

	for (j=0; j<kFlashRowSizeInstructions; ++j)
	{
		unsigned int low = (row_buffer[j*3+1] << 8) | row_buffer[j*3];
		unsigned int high = row_buffer[j*3+2];

		__builtin_tblwtl((row*kFlashRowSizeWords + j*2) & 0xFFFF, low);
		__builtin_tblwth((row*kFlashRowSizeWords + j*2) & 0xFFFF, high);
	}

	flash_operation(kFlashWriteRow);
}

void _BOOTLOADER_CODE erase_row(unsigned int row)
{
	TBLPAG = 0; //For devices with <= 64kb of RAM, we don't need to page.
	
	__builtin_tblwtl((row*kFlashRowSizeWords) & 0xFFFF, 0);
	flash_operation(kFlashEraseRow);
}

void _BOOTLOADER_CODE program_application(unsigned int sector)
{
	uint32 	addr = MEMORY_SECTION_ADDR(sector);
	unsigned int i;
	unsigned char row_buffer[kFlashRowSizeInstructions*3];

	uint32 reset_low, reset_high;

	//Enable the Memory Module
    _RB7 = 1;
    _TRISB7 = 0;
	configure_SPI();

	//Give the flash memory time to warm up. (it needs at least 30 us after VCC reaches min value)
	DELAY_MS(1);

	//Read in all of the rows from flash and program ourselves
	for (i=0; i<kNumFirmwareRows; ++i)
	{
		mem_read(addr, row_buffer, kFlashRowSizeInstructions*3);

		//Save and patch the application goto vector and replace the reset vector
		//with a jump to us.
		if (i == 0)
		{
			extract_reset_vector(row_buffer, &reset_low, &reset_high);
			patch_reset_vector(row_buffer, kBootloaderGotoLow, kBootloaderGotoHigh);
		}
		else if (i == kAppJumpRow)
			patch_reset_vector(row_buffer, reset_low, reset_high);

		erase_row(i);
		write_row(i, row_buffer);
	}
}

bool _BOOTLOADER_CODE valid_instruction(unsigned int addr)
{
	unsigned int low, high;

	TBLPAG = 0;
	low = __builtin_tblrdl(addr);
	high = __builtin_tblrdh(addr);

	if (low == 0xFFFF && ((high & 0xFF) == 0xFF))
		return false;

	return true;
}

void _BOOTLOADER_CODE extract_reset_vector(unsigned char *row_buffer, uint32 *low, uint32 *high)
{
	*low = 0;
	*low |= row_buffer[0];
	*low |= ((uint32)row_buffer[1]) << 8;
	*low |= ((uint32)row_buffer[2]) << 16;

	*high = 0;
	*high |= row_buffer[3];
	*high |= ((uint32)row_buffer[4]) << 8;
	*high |= ((uint32)row_buffer[5]) << 16;
}

void _BOOTLOADER_CODE patch_reset_vector(unsigned char *row_buffer, uint32 low, uint32 high)
{
	row_buffer[0] = (low >> 0) & 0xFF;
	row_buffer[1] = (low >> 8) & 0xFF;
	row_buffer[2] = (low >> 16) & 0xFF;

	row_buffer[3] = (high >> 0) & 0xFF;
	row_buffer[4] = (high >> 8) & 0xFF;
	row_buffer[5] = (high >> 16) & 0xFF;
}

void _BOOTLOADER_CODE goto_address(unsigned int addr)
{
	asm volatile ("goto w0");
}