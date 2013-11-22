//test_log.as

#include <xc.inc>

#define CONTROLREG 	BANKMASK(CCPR1L)
#define CONTROLBANK CCPR1L

#define LOGREG 		BANKMASK(CCPR1H)
#define LOGBANK		CCPR1H

#define FINISH 		0x00
#define LOGHEX		0x01
#define LOGSTRING	0x02
#define LOGCALLER 	0x03

global _loghex, _finish_tests

;These must be in common ram since we cannot switch banks
;while accessing them
PSECT bssCOMMON,global,class=COMMON,delta=1
banksave: ds 1
regsave: ds 1
fsrlsave: ds 1
fsrhsave: ds 1
intsave: ds 1

PSECT text_unittest,local,class=CODE,delta=2

;Save all of the registers that these routines could overwrite
_savestate:
	movwf 	regsave
	movf  	BSR,w
	movwf 	banksave
	movf 	FSR0L,w
	movwf	fsrlsave
	movf 	FSR0H,w
	movwf	fsrhsave

	movf 	INTCON,w
	movwf	intsave
	bcf 	GIE		;disable interrupts during logging

	return

;Restore all registers to the state they were in
;before we were called
_restorestate:
	movf 	banksave,w
	movwf 	BSR
	movf 	fsrlsave,w
	movwf 	FSR0L
	movf 	fsrhsave,w
	movwf 	FSR0H
	movf 	intsave,w
	movwf 	INTCON
	return

;helper function for logging the address of the instruction that
;called the parent of this function
logcaller:
	;send control char
	banksel CONTROLBANK
	movlw 	LOGCALLER
	movwf 	CONTROLREG

	banksel STKPTR
	decf	BANKMASK(STKPTR),f
	movf 	BANKMASK(TOSL),w
	banksel CONTROLBANK
	movwf	LOGREG

	banksel STKPTR
	movf 	BANKMASK(TOSH),w
	banksel CONTROLBANK
	movwf 	LOGREG

	banksel STKPTR
	incf	BANKMASK(STKPTR),f

	return

;Log the value of the w register into the unit test logging register
;Uses: WREG
_loghex:
	call	_savestate
	call	logcaller

	banksel CONTROLBANK
	movlw 	LOGHEX
	movwf 	CONTROLREG

	;save the value
	movf 	regsave,w
	banksel LOGBANK
	movwf 	LOGREG

	goto _restorestate

_finish_tests:
	banksel CONTROLBANK
	movlw FINISH
	movwf CONTROLREG
	return