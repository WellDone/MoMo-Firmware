#include <xc.inc>
#include "asm_macros.inc"
#include "executive.h"

global _exec_prepare_reflash, _exec_reset, _exec_verify, _exec_readmem, _exec_status

PSECT textexecmap,local,class=CODE,delta=2

;This file defines the commands that the pic12 mib executive answers on behalf of its application
;without forwarding them on.  

define_endpoint MACRO id, address
retlw (id && 0xFF)
retlw (id shr 8) && 0xFF
retlw address && 0xFF
retlw (address shr 8) && 0xFF
ENDM

;Endpoints must be defined in order!
BEGINREGION exec_new_cmd_map
	define_endpoint kExecutivePrepareReflash, _exec_prepare_reflash
	define_endpoint kExecutiveReset, _exec_reset
	define_endpoint kExecutiveVerifyApplication, _exec_verify
	define_endpoint kExecutiveReadMemory, _exec_readmem
	define_endpoint kExecutiveStatus, _exec_status
	define_endpoint 0xFFFF, 0xFFFF
ENDREGION exec_new_cmd_map
