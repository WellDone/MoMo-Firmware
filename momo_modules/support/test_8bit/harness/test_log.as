//test_log.as

#include <xc.inc>

#define CONTROLREG 	BANKMASK(CCPR1L)
#define CONTROLBANK CCPR1L

#define LOGREG 		BANKMASK(CCPR1H)
#define LOGBANK		CCPR1H

#define FINISH 		0x00
#define LOGHEX		0x01
#define LOGSTRING	0x02

global _loghex, _finish_tests, _assert_address

PSECT bssCOMMON,class=COMMON,delta=1
banksave: ds 1
regsave: ds 1
fsrlsave: ds 1
fsrhsave: ds 1
_assert_address: ds 1

PSECT text_unittest,local,class=CODE,delta=2

;Log the value of the w register into the unit test logging register
;Uses: WREG
_loghex:
	movwf 	regsave
	movf  	BSR,w
	movwf 	banksave
	banksel CONTROLBANK
	movlw 	LOGHEX
	movwf 	CONTROLREG

	;save the value
	movf 	regsave,w
	banksel LOGBANK
	movwf 	LOGREG

	movf 	banksave,w
	movwf 	BSR
	return

;Assert that the register (in the current bank) specified in ram _assert_address is equal to w
_assert:
	

_finish_tests:
	banksel CONTROLBANK
	movlw FINISH
	movwf CONTROLREG
	return