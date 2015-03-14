;test_log.as

#include <xc.inc>

#define CONTROLREG 	BANKMASK(CCPR1L)
#define CONTROLBANK CCPR1L

#define LOGREG 		BANKMASK(CCPR1H)
#define LOGBANK		CCPR1H

#define FINISH 		0x00
#define LOGHEX		0x01
#define LOGSTRING	0x02
#define LOGCALLER 	0x03
#define ASSERTFAIL 	0x04
#define CHECKPOINT  0x05

global _loghex, _finish_tests, _assertv, _checkpoint, _checkpoint_value

;These must be in common ram since we cannot switch banks
;while accessing them
PSECT bssCOMMON,global,class=COMMON,delta=1
banksave: ds 1
regsave: ds 1
fsrlsave: ds 1
fsrhsave: ds 1

PSECT text_unit1,local,class=CODE,delta=2

;Save all of the registers that these routines could overwrite
_savestate:
	movwf 	regsave
	movf  	BSR,w
	movwf 	banksave
	movf 	FSR0L,w
	movwf	fsrlsave
	movf 	FSR0H,w
	movwf	fsrhsave
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

;For asserts, the value we're comparing should be in FSR0L and the
;value that it should be will be in W
_assertv:
	call _savestate
	call logcaller

	movf regsave,w
	xorwf FSR0L,w
	btfss ZERO
	goto  assert_failed

	call _restorestate
	return
	
	assert_failed:
	movlw ASSERTFAIL
	call _emit_control

	;Emit actual value and then value that we wanted
	movf FSR0L,w
	call _emit_data

	movf regsave,w
	call _emit_data

	goto _finish_tests

;_checkpoint_value
;Log that a certain function was called and the value of W at that point
_checkpoint_value:
	call _savestate
	call logcaller
	
	movlw CHECKPOINT
	call _emit_control

	movf regsave,w
	call _emit_data


	goto _restorestate

;Log that a certain function was called but do not log the value of W
;in case it is unpredictable and not important.
_checkpoint:
	call _savestate
	call logcaller
	
	movlw CHECKPOINT
	call _emit_control

	movlw 0
	call _emit_data

	goto _restorestate

;Log the value of the w register into the unit test logging register
;Uses: WREG
_loghex:
	call	_savestate
	call	logcaller
	

	;set log type
	movlw 	LOGHEX
	call _emit_control

	;save the value
	movf 	regsave,w
	call _emit_data

	goto _restorestate

;Emit the W value into the control register
_emit_control:
	banksel CONTROLBANK
	movwf 	CONTROLREG
	return

_emit_data:
	banksel LOGBANK
	movwf 	LOGREG
	return

_finish_tests:
	call logcaller
	banksel CONTROLBANK
	movlw FINISH
	movwf CONTROLREG
	return