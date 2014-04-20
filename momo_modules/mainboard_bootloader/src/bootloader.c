//bootloader.c

#include "bootloader.h"
#include "memory.h"
#include "ioport.h"
#include "constants.h"

void _BOOTLOADER_CODE write_row(unsigned int row, unsigned char *row_buffer)
{
	unsigned int j;

	TBLPAG = 0;

	for (j=0; j<kFlashRowSizeInstructions; ++j)
	{
		unsigned int low = (((unsigned int)row_buffer[j*3+1]) << 8) | ((unsigned int)row_buffer[j*3]);
		unsigned int high = row_buffer[j*3+2];

		__builtin_tblwtl((row*kFlashRowSizeWords + j*2) & 0xFFFF, low);
		__builtin_tblwth((row*kFlashRowSizeWords + j*2) & 0xFFFF, high);
	}

	flash_operation(kFlashWriteRow);
}

void _BOOTLOADER_CODE erase_page(unsigned int page)
{
	TBLPAG = 0; //For devices with <= 64kb of RAM, we don't need to page.
	
	__builtin_tblwtl((page*kFlashPageSizeWords) & 0xFFFF, 0);
	flash_operation(kFlashErasePage);
}

void _BOOTLOADER_CODE program_application(unsigned int sector)
{
	uint32 	addr = MEMORY_SECTION_ADDR(sector);
	unsigned int row, page, i;
	unsigned char row_buffer[kFlashRowSizeInstructions*3];

	uint32 reset_low, reset_high;

	extract_reset_vector(&reset_low, &reset_high);

	//Enable the Memory Module
    LAT(E5) = 1;
    DIR(E5) = OUTPUT;
	configure_SPI();

	//Give the flash memory time to warm up. (it needs at least 30 us after VCC reaches min value)
	DELAY_MS(1);

	for (page=0, row=0; page<kNumFirmwarePages; ++page)
	{
		erase_page(page);

		//Read in all of the rows from flash for this page and program them
		for (i=0; i<kNumRowsPerPage; ++i)
		{
			mem_read(addr, row_buffer, kFlashRowSizeInstructions*3);

			//Patch the application goto vector and replace the reset vector
			//with a jump to us.
			if (row == 0)
				patch_reset_vector(row_buffer, reset_low, reset_high);
			else if(row == kAppJumpRow)
			{
				//Overwrite the flash at address 0x100 so that we know we have a valid application
				row_buffer[0] = 0xAA;
				row_buffer[1] = 0xAA;
				row_buffer[2] = 0xAA;
				row_buffer[3] = 0xAA;
				row_buffer[4] = 0xAA;
				row_buffer[5] = 0xAA;
			}

			write_row(row, row_buffer);
			++row;

			addr += kFlashRowSizeInstructions*3;
		}
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

void _BOOTLOADER_CODE extract_reset_vector(uint32 *low, uint32 *high)
{
	unsigned int tmpl, tmph;

	TBLPAG = 0;
	tmpl = __builtin_tblrdl(0);
	tmph = __builtin_tblrdh(0);

	*low = 0;
	*low |= tmpl;
	*low |= ((uint32)tmph & 0xFF) << 16;

	tmpl = __builtin_tblrdl(2);
	tmph = __builtin_tblrdh(2);

	*high = 0;
	*high |= tmpl;
	*high |= ((uint32)tmph & 0xFF) << 16;
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