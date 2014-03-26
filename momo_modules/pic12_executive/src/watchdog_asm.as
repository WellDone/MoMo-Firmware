;watchdog.as

#include <xc.inc>
#include "asm_macros.inc"

GLOBAL _status
global	start

PSECT wdt_utils,local,class=CODE,delta=2

BEGINFUNCTION _wdt_settimeout
	banksel WDTCON
	movwf BANKMASK(WDTCON)
	return
ENDFUNCTION _wdt_settimeout

BEGINFUNCTION _wdt_delay
	call _wdt_settimeout
	bsf SWDTEN
	sleep
	bcf SWDTEN
	return
ENDFUNCTION _wdt_delay

;We need to save off the status of the reset just after powerup
;so that we know if it was due to a watchdog reset
psect powerup,class=CODE,delta=2

BEGINREGION powerup
	BANKSEL _status
	clrf BANKMASK(_status)
	btfss nTO
	bsf BANKMASK(_status), 4
	bsf nTO
	goto start
ENDREGION powerup