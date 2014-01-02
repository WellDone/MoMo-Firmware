#ifndef __bootloader_h__
#define __bootloader_h__

#include "bootloader_defs.h"

#define kHighWordAddress		0x7FF
#define kAppSourceAddress		0x7FE
#define kRetlwHighWord			0b110100

#define set_flash_word(offset, low, high) 	{app_buffer[offset << 1] = low; app_buffer[(offset << 1) + 1] = high;}

#ifndef _DEFINES_ONLY
#include "platform.h"
#include "flash_memory.h"

void 	set_firmware_id(uint8 bucket);
void 	prepare_reflash(uint8 source);
void 	enter_bootloader();
void 	get_half_row(uint8 offset);

typedef union
{
	struct
	{
		uint8 wdt_enabled	: 1; //cannot change position (referenced by watchdog.as)
		uint8 valid_app 	: 1;
		uint8 bootload_mode : 1;
		uint8 registered	: 1;
		uint8 wdt_timedout	: 1; //cannot change, referenced in watchdog.as
		uint8 reserved 		: 3; 
	};

	uint8 status;
} MIBExecutiveStatus;

#endif


#endif