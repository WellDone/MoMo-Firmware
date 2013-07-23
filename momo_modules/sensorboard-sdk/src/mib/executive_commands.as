#include <xc.inc>

global _exec_get_spec, _exec_call_cmd

PSECT text_executive_map,local,class=CODE,delta=2

;This file defines the commands that the pic12 mib executive answers on behalf of its application
;without forwarding them on.  

exec_spec_map:
brw
retlw 0

exec_cmd_map:
brw
goto INVALID_HANDLER

_exec_get_spec:
	goto exec_spec_map

_exec_call_cmd:
	goto exec_cmd_map