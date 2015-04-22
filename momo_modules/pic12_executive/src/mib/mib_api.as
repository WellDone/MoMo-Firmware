;mib_api.as

#include <xc.inc>
#include "constants.h"
#include "asm_macros.inc"

PSECT mibapi,global,class=CODE,delta=2

global _main

;Use the final high words of the mib_executive rom for api callbacks
;mib api lives in the last 16 bytes before the application.
;the currently used api spots are filled with goto _main because
;xc8 dies when trying to figure out the compiled stack with references
;to other functions lying around in memory.  The real addresses are
;patched in
BEGINREGION mib12_api
	retlw 0
	retlw 0
	retlw 0
	retlw 0

	retlw 0
	retlw 0
	retlw 0
	retlw 0

	retlw 0
	retlw 0
	retlw 0
	goto _main	;reset()

	goto _main  ;trap(uint8 code)
	goto _main  ;bus_master_rpc_begin(addr)
	goto _main	;bus_master_rpc_send()
	goto _main	;bus_slave_return_data(length)
ENDREGION mib12_api