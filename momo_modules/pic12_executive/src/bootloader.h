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

//1 Byte
typedef union
{
	struct
	{
		uint16 reserved1 		: 1; //cannot change position (referenced by watchdog_asm.as)
		uint16 valid_app 		: 1;
		uint16 bootload_mode 	: 1;
		uint16 registered		: 1;
		uint16 dirty_reset 		: 1; //cannot change, referenced in watchdog_asm.as
		uint16 slave_active 	: 1; //cannot change, referenced in i2c_utilities.as
		uint16 first_read		: 1;
		uint16 trapped			: 1;
	};

	uint16 status;
} MIBExecutiveStatus;

#endif


#endif