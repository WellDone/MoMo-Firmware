#include <xc.inc>
#include "asm_macros.inc"

#define plist_ints(count)		((count&0b11) << 5)
#define plist_buffer()          0b10000000

#define plist_spec(ni,buffer) (buffer << 7) | plist_ints(ni) )

global _exec_prepare_reflash,_exec_reset,_exec_verify

PSECT textexecmap,local,class=CODE,delta=2

;This file defines the commands that the pic12 mib executive answers on behalf of its application
;without forwarding them on.  

BEGINREGION exec_cmd_map
	brw
	goto _exec_prepare_reflash
	goto _exec_reset
	goto _exec_verify
ENDREGION exec_cmd_map

BEGINREGION exec_spec_map
	brw
	retlw (2 << 5)
	retlw 0
	retlw 0
ENDREGION exec_spec_map

BEGINFUNCTION _exec_get_spec
	goto exec_spec_map
ENDFUNCTION _exec_get_spec

BEGINFUNCTION _exec_call_cmd
	goto exec_cmd_map
ENDFUNCTION _exec_call_cmd