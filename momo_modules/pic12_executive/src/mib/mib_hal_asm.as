#include <xc.inc>
#include "executive.h"
#include "constants.h"
#include "asm_macros.inc"
#include "asm_branches.inc"

;mib_params.asm
;Assembly routines for dealing with MIB things in an efficient way
;very quickly and with minimal code overhead

GLOBAL _mib_state, _mib_buffer,_bus_slave_seterror
GLOBAL _exec_call_cmd, _exec_get_spec, _find_handler

PSECT text100,local,class=CODE,delta=2

mibfeature equ BANKMASK(_mib_state+0)
mibcmd	   equ BANKMASK(_mib_state+1)
mibspec	   equ BANKMASK(_mib_state+2)

#define kInvalidMIBIndex 255

EXPAND

;Given a MIB command passed in the MIB packet, decide if
;we have a handler for that feature,command pair
;Uses: W, FSR0L,FSR0H
BEGINFUNCTION _find_handler
	movlb 1
	;skipnelf mibfeature,kMIBExecutiveFeature
		movf mibfeature,w
		xorlw kMIBExecutiveFeature
		btfss ZERO
	goto  notexecutive
	;Check if the command is less than the number of executive commands
	;skipltfl mibcmd, kNumExecutiveCommands
		movlw kNumExecutiveCommands
		subwf mibcmd,w
		btfsc CARRY
	retlw kInvalidMIBIndex
	retf mibcmd

	;Check if there is an application module loaded and see if it
	;defines this endpoint
	notexecutive:
	call _get_magic
	xorlw kMIBMagicNumber
	;skipz
		btfss ZERO
	retlw kInvalidMIBIndex

	;iterate through all of the features seeing if this matches
	movlw 0
	movwf FSR0L
	feature_loop:
	call _get_num_features
	;skipltfw FSR0L	;return if current feature >= get_num_features()
	subwf FSR0L,w
	btfsc CARRY
		retlw kInvalidMIBIndex
	movf FSR0L,w
	call _get_feature
	;skipnewf mibfeature
		xorwf mibfeature,w
		btfsc ZERO
	goto found_feature
	incf FSR0L,f
	goto feature_loop

	found_feature:
	movf FSR0L,w
	movwf FSR0H
	call _get_command
	addwf mibcmd,w
	movwf FSR0L			;FSR0L now has cmd+get_command(found_feat)
	incf  FSR0H,w
	call _get_command 	;W now has get_command(found_feat+1)
	;skipgefw FSR0L
		subwf FSR0L,w
		btfsc CARRY		;This is CARRY NOT BORROW, we want ~BORROW
		retlw kInvalidMIBIndex

	movf FSR0L,w
	return
ENDFUNCTION _find_handler

;Given an index into the handler table in application code, call that handler
;if the command is handled by the executive, jump to the executive table
BEGINFUNCTION _call_handler
	movwf FSR1L
	movlb 1
	movlw kMIBExecutiveFeature
	xorwf BANKMASK(_mib_state+0),w ;see if feature matches kMIBExecutiveFeature
	btfss ZERO
	GOTO call_app
	movf FSR1L,w
	GOTO _exec_call_cmd
	call_app:
	movf FSR1L,w
	GOTO 0x7FE				;branch to the goto in high memory that redirects to the application code's mib callback table
ENDFUNCTION _call_handler

BEGINFUNCTION _get_feature
	GOTO 0x7FB
ENDFUNCTION _get_feature

BEGINFUNCTION _get_command
	GOTO 0x7FC
ENDFUNCTION _get_command

BEGINFUNCTION _get_param_spec
	movwf FSR1L
	movlb 1
	movlw kMIBExecutiveFeature
	xorwf BANKMASK(_mib_state+0),w ;see if feature matches kMIBExecutiveFeature
	btfss ZERO
	GOTO call_spec
	movf FSR1L,w
	GOTO _exec_get_spec
	call_spec:
	movf FSR1L,w
	GOTO 0x7FD
ENDFUNCTION _get_param_spec

;Given a word offset in the mib module definition block
;in w, return the value at that address
BEGINFUNCTION _get_mib_block
	movwf FSR1L
	movlw low kMIBEndpointAddress
	addwf FSR1L
	movlw 0x87
	movwf FSR1H
	movf  INDF1,w
	return
ENDFUNCTION _get_mib_block


;indirect read the highest byte from program memory
BEGINFUNCTION _get_magic
	movlw kMIBMagicOffset
	goto  _get_mib_block		;tail call
ENDFUNCTION _get_magic

BEGINFUNCTION _get_num_features
	GOTO 0x7FA
ENDFUNCTION _get_num_features

BEGINFUNCTION _plist_int_count
	andlw 0b01100000	; ((plist & 0b01100000) >> 5)
	swapf WREG,w
	lsrf  WREG,w
ENDFUNCTION _plist_int_count

; compute ((plist_int_count(plist) << 1) + (plist & plist_buffer_mask))
; efficiiently on the pic12
BEGINFUNCTION _plist_param_length
	movlb 3
	movwf BANKMASK(EEADRL)  					
	andlw 0b01100000
	swapf WREG,w
	movwf BANKMASK(EEDATL)	;contains int size
	movlw 0b00011111
	andwf BANKMASK(EEADRL),w
	addwf BANKMASK(EEDATL),w
	movlb 0
	return
ENDFUNCTION _plist_param_length

; given the handler index in W, validate that the passed params match the spec
BEGINFUNCTION _validate_param_spec
	call _get_param_spec		;handler spec in w
	movlb 1
	xorwf BANKMASK(_mib_state+2),w ;passed spec xor handler spec
	andlw 0b11100000 				;only look at spec, ignore length
	btfsc ZERO						;if they match w should be zero
	retlw 1 
	retlw 0
ENDFUNCTION _validate_param_spec

;bus_returnstatus is at mib_state+7
BEGINFUNCTION _bus_slave_seterror
	movlb 1
	clrf BANKMASK(_mib_state+7)
	swapf WREG,w  					;error << 4
	lslf WREG,w 					; error << 1 
	iorwf BANKMASK(_mib_state+7),f 	;return_status |= error << 5
	bsf BANKMASK(_mib_state+9), 2 	;we want bits 2 and 3 set.  (set slave_state to 3)
	bsf BANKMASK(_mib_state+9), 3
	return
ENDFUNCTION _bus_slave_seterror
