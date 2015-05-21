;watchdog.as

#include <xc.inc>
#include "asm_locations.h"

ASM_INCLUDE_GLOBALS()

global	start, _wdt_delay

PSECT wdt_utils,local,class=CODE,delta=2

BEGINFUNCTION _wdt_delay
	banksel WDTCON
	movwf BANKMASK(WDTCON)
	bsf SWDTEN
	sleep
	bcf SWDTEN
	return
ENDFUNCTION _wdt_delay


psect powerup,class=CODE,delta=2
;Check if we reset due to a RESET instruction and
;if so, see if it was due to an API call, if not,
;save this off so we can trap() and debug the error.
BEGINREGION powerup
	;Clear all bits but the DirtyResetBit
	BANKSEL _status
	movlw 0
	btfss nRI
		movlw (1<<DirtyResetBit)
	bsf nRI
	andwf BANKMASK(_status)
	goto start
ENDREGION powerup