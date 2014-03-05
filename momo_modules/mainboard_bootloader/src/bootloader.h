#ifndef __bootloader_h__
#define __bootloader_h__

#include "pic24.h"

#define _BOOTLOADER_CODE __attribute__((section(".bootloader")))

enum
{
	kFlashWriteRow = 0b00000100,
	kFlashEraseRow = 0b01011000
}; 

void zero_application();
void erase_row(unsigned int row);

//Assembly Instructions
extern void flash_operation(unsigned int nvmcon);

#endif