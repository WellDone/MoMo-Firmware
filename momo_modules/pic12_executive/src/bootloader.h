#ifndef __bootloader_h__
#define __bootloader_h__

#include "bootloader_defs.h"

#define kRetlwHighWord			0b110100

#define set_flash_word(offset, low, high) 	{app_buffer[offset << 1] = low; app_buffer[(offset << 1) + 1] = high;}

#ifndef _DEFINES_ONLY
#include "platform.h"
#include "flash_memory.h"

void 	set_firmware_id(uint8 bucket);
void 	prepare_reflash(uint8 source);
void 	enter_bootloader();
void 	get_half_row();

#endif


#endif