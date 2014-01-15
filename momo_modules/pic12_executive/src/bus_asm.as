#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "constants.h"
#define __DEFINES_ONLY__
#include "mib_definitions.h"
#undef  __DEFINES_ONLY__

ASM_INCLUDE_GLOBALS()

global _bus_master_rpc_sync, _copy_mib_to_boot
global _boot_id, _invalid_row, _boot_source, _offset

PSECT text_bootloader_asm,local,class=CODE,delta=2

BEGINFUNCTION _bus_init

ENDFUNCTION _bus_init