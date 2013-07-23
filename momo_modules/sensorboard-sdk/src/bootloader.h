#ifndef __bootloader_h__
#define __bootloader_h__

#define kFirstApplicationRow	78
#define kNumFlashRows			128
#define kFlashRowSize			16		//words
#define kBootloaderBufferLoc	0x20	//bootloader uses first 32 bytes of ram in bank0 to cache flash row
#define kHighWordAddress		0x7FF
#define kAppSourceAddress		0x7FE
#define kReflashMagicNumber		0x54
#define kRetlwHighWord			0b110100

#define set_flash_word(offset, low, high) 	{app_buffer[offset << 1] = low; app_buffer[(offset << 1) + 1] = high;}

#ifndef _DEFINES_ONLY
#include "platform.h"
#include "flash_memory.h"

void prepare_reflash(uint8 source);
void enter_bootloader();
#endif

#endif