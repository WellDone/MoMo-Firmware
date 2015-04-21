#include <xc.inc>
#include "asm_macros.inc"

global _exec_prepare_reflash, _exec_reset, _exec_verify, _exec_readmem, _exec_status

PSECT textexecmap,local,class=CODE,delta=2

;This file defines the commands that the pic12 mib executive answers on behalf of its application
;without forwarding them on.  

BEGINREGION exec_cmd_map
	brw
	goto _exec_prepare_reflash
	goto _exec_reset
	goto _exec_verify
	goto _exec_readmem
	goto _exec_status
ENDREGION exec_cmd_map

BEGINREGION exec_new_cmd_map
	retlw 0xFF
	retlw 0xFF
	retlw 0xFF
	retlw 0xFF
ENDREGION exec_new_cmd_map

BEGINREGION exec_spec_map
	brw
	retlw (2 << 5)
	retlw 0
	retlw 0
	retlw (1 << 5)
	retlw 0
ENDREGION exec_spec_map