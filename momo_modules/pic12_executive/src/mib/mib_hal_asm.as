#include <xc.inc>
#include "executive.h"
#include "constants.h"
#include "asm_locations.h"
#include "i2c_defines.h"
#include "asm_branches.inc"

;mib_params.asm
;Assembly routines for dealing with MIB things in an efficient way
;very quickly and with minimal code overhead

ASM_INCLUDE_GLOBALS()
GLOBAL exec_cmd_map, exec_spec_map, _find_handler

PSECT text100,local,class=CODE,delta=2

;Given a MIB command passed in the MIB packet, decide if
;we have a handler for that feature,command pair
;Uses: W, FSR0L,FSR0H
BEGINFUNCTION _find_handler
	banksel(bus_feature)
	;skipnelf BANKMASK(bus_feature),kMIBExecutiveFeature
		movf BANKMASK(bus_feature),w
		xorlw kMIBExecutiveFeature
		btfss ZERO
	goto  notexecutive
	;Check if the command is less than the number of executive commands
	;skipltfl BANKMASK(bus_command), kNumExecutiveCommands
		movlw kNumExecutiveCommands
		subwf BANKMASK(bus_command),w
		btfsc CARRY
	retlw kInvalidMIBIndex
	retf BANKMASK(bus_command)

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
	;skipnewf BANKMASK(bus_feature)
		xorwf BANKMASK(bus_feature),w
		btfsc ZERO
	goto found_feature
	incf FSR0L,f
	goto feature_loop

	found_feature:
	movf FSR0L,w
	movwf FSR0H
	call _get_command
	addwf BANKMASK(bus_command),w
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
;handler is passed in W register
BEGINFUNCTION _call_handler
	banksel (bus_feature) 
	movwf FSR1L
	movlw kMIBExecutiveFeature
	xorwf BANKMASK(bus_feature),w ;see if feature matches kMIBExecutiveFeature
	btfss ZERO
	GOTO call_app
	movf FSR1L,w
	GOTO exec_cmd_map
	call_app:
	movf FSR1L,w

	call 0x7FE				;branch to the goto in high memory that redirects to the application code's mib callback table
	reset_page()
	return 

ENDFUNCTION _call_handler

BEGINFUNCTION _get_feature
	call 0x7FB
	reset_page()
	return
ENDFUNCTION _get_feature

BEGINFUNCTION _get_command
	call 0x7FC
	reset_page()
	return 
ENDFUNCTION _get_command

;Given a word offset in the mib module definition block
;in w, return the value at that address
;NOTES: 
; - Must not change the bank
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
	call 0x7FA
	reset_page()
	return
ENDFUNCTION _get_num_features

