;watchdog.as

#include <xc.inc>

GLOBAL _status
GLOBAL _wdt_pushenabled, _wdt_popenabled
global	powerup,start

PSECT wdt_utils,local,class=CODE,delta=2

;disable the wdt and remember if it was enabled or not
;before the disabling occured.
_wdt_pushenabled:
	BANKSEL WDTCON
	movf WDTCON,w
	andlw 1
	bcf SWDTEN
	BANKSEL _status
	bcf BANKMASK(_status),0
	iorwf BANKMASK(_status),f
	return

_wdt_popenabled:
	BANKSEL _status
	btfss BANKMASK(_status),0
	goto done
	BANKSEL WDTCON
	bsf SWDTEN
	done:
	return

;We need to save off the status of the reset just after powerup
;so that we know if it was due to a watchdog reset
psect powerup,class=CODE,delta=2
powerup:

BANKSEL _status
clrf BANKMASK(_status)
btfss nTO
bsf BANKMASK(_status), 4
bsf nTO
goto start