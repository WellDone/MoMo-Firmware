;Assembly routines used for interacting with eeprom on PIC12 and PIC16
;devices.  
; Notes and Caveats
; - only up to 256 bytes are supported.


#include <xc.inc>
#define __DEFINES_ONLY__
#include "mib_definitions.h"
#define _DEFINES_ONLY
#include "bootloader.h"
#undef  __DEFINES_ONLY__
#include "i2c_defines.h"
#include "asm_locations.h"
#include "constants.h"


ASM_INCLUDE_GLOBALS()

global _bus_master_rpc_sync, _copy_mib_to_boot, _load_boot_address
global _boot_id, _invalid_row, _boot_source, _offset, _app_buffer
global _flash_erase_row, _flash_write_row

PSECT text_bootloader_asm,local,class=CODE,delta=2