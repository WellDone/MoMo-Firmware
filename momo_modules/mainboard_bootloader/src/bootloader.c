//bootloader.c

#include "bootloader.h"
#include "memory.h"
#include "constants.h"

void _BOOTLOADER_CODE zero_application()
{
	unsigned int i, j;

	for (i=0; i<kNumFirmwareRows;++i)
	{
		erase_row(i);

		for (j=0; j<kFlashRowSize; ++j)
		{
			__builtin_tblwtl((i*kFlashRowSize + j) & 0xFFFF, 0);
			__builtin_tblwth((i*kFlashRowSize + j) & 0xFFFF, 0);
		}

		flash_operation(kFlashWriteRow);
	}
}

void _BOOTLOADER_CODE erase_row(unsigned int row)
{
	TBLPAG = 0; //For devices with <= 64kb of RAM, we don't need to page.
	
	__builtin_tblwtl((row*kFlashRowSize) & 0xFFFF, 0);
	flash_operation(kFlashEraseRow);
}