
#include <xc.inc>
#define __DEFINES_ONLY__
#include "mib_definitions.h"
#define _DEFINES_ONLY
#include "bootloader.h"
#undef  __DEFINES_ONLY__
#include "i2c_defines.h"
#include "asm_locations.h"

ASM_INCLUDE_GLOBALS()

global _call_app_init, _call_app_interrupt, _call_app_task

PSECT app_vectors,global,class=CODE,delta=2
_call_app_init:
retlw 0

_call_app_interrupt:
retlw 0

_call_app_task:
retlw 0