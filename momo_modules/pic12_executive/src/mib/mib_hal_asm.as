#include <xc.inc>
#include "executive.h"
#include "constants.h"
#include "asm_macros.inc"
#include "i2c_defines.h"
#include "asm_branches.inc"

;mib_params.asm
;Assembly routines for dealing with MIB things in an efficient way
;very quickly and with minimal code overhead

GLOBAL exec_cmd_map, exec_spec_map, _find_handler, _bus_is_idle
global _mib_data

PSECT text100,local,class=CODE,delta=2

mibfeature equ BANKMASK(_mib_data+0)
mibcmd	   equ BANKMASK(_mib_data+1)
mibspec	   equ BANKMASK(_mib_data+2)

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
	xorwf mibfeature,w ;see if feature matches kMIBExecutiveFeature
	btfss ZERO
	GOTO call_app
	movf FSR1L,w
	GOTO exec_cmd_map
	call_app:
	movf FSR1L,w

	call 0x7FE				;branch to the goto in high memory that redirects to the application code's mib callback table
	pagesel($)
	return 

ENDFUNCTION _call_handler

BEGINFUNCTION _get_feature
	call 0x7FB
	pagesel($)
	return
ENDFUNCTION _get_feature

BEGINFUNCTION _get_command
	call 0x7FC
	pagesel($)
	return 
ENDFUNCTION _get_command

BEGINFUNCTION _get_param_spec
	movwf FSR1L
	movlb 1
	movlw kMIBExecutiveFeature
	xorwf mibfeature,w ;see if feature matches kMIBExecutiveFeature
	btfss ZERO
	GOTO call_spec
	movf FSR1L,w
	GOTO exec_spec_map
	
	call_spec:
	movf FSR1L,w
	call 0x7FD
	pagesel($)
	return
ENDFUNCTION _get_param_spec

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
	pagesel($)
	return
ENDFUNCTION _get_num_features

; compute ((plist_int_count(plist) << 1) + (plist & plist_buffer_mask))
; efficiently on the pic12
; Uses: FSR0L and FSR0H
BEGINFUNCTION _plist_param_length
	banksel _mib_data
	movf 	mibspec,w
	movwf BANKMASK(FSR0L)  					
	andlw 0b01100000
	swapf WREG,w
	movwf BANKMASK(FSR0H)	;contains int size
	movlw 0b00011111
	andwf BANKMASK(FSR0L),w
	addwf BANKMASK(FSR0H),w
	return
ENDFUNCTION _plist_param_length

; given the handler index in W, validate that the passed params match the spec
BEGINFUNCTION _validate_param_spec
	call _get_param_spec		;handler spec in w
	movlb 1
	xorwf mibspec,w ;passed spec xor handler spec
	andlw 0b11100000 				;only look at spec, ignore length
	btfsc ZERO						;if they match w should be zero
		retlw 1 
	retlw 0
ENDFUNCTION _validate_param_spec


;uint8 bus_is_idle()
;{
;	//bus is not idle if a start condition has been detected with no stop.
;	if (SSPSTATbits.S)
;		return 0;

;	//If a stop has been asserted with no starts, the bus is idle
;	if (SSPSTATbits.P)
;		return 1;
;	else if (SDAPIN && SCLPIN) //If no starts and no stops and the pins are high, it's idle (only happens right after a reset before the first transmission)
;			return 1;

;	//Otherwise it is not idle
;	return 0;
;}
BEGINFUNCTION _bus_is_idle
	banksel SSP1STAT
	btfsc BANKMASK(SSP1STAT), SSP1STAT_S_POSN
		retlw 0
	btfsc BANKMASK(SSP1STAT), SSP1STAT_P_POSN
		retlw 1
	banksel I2CPORT
	movf 	BANKMASK(I2CPORT),w
	andlw	I2CMASK
	xorlw 	I2CMASK		;zero bit will be set if SCL and SDA are high
	btfsc 	ZERO
		retlw 1
	retlw 0
ENDFUNCTION _bus_is_idle