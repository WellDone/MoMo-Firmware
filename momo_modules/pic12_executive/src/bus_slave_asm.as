;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"
#include "protocol_defines.h"

ASM_INCLUDE_GLOBALS()

global _i2c_calculate_checksum
global _find_handler, _call_handler

PSECT text_bus_slave,local,class=CODE,delta=2

;Given the returnvalue in WREG set the bus slave return status to that value
;and update the bus status checksum to the correct checksum value.
;Uses:WREG, FSR0L
;Returns:Nothing
;Modifies: Z,bank1
BEGINFUNCTION _bus_slave_setreturn
	banksel bus_status
	movwf 	BANKMASK(bus_status)
	comf	WREG,w
	incf	WREG,w
	movwf 	BANKMASK(bus_statuscheck)
	return
ENDFUNCTION _bus_slave_setreturn

BEGINFUNCTION _bus_slave_callcommand
	call _i2c_calculate_checksum
	skipz
		goto checksum_error

	call _find_handler
	movwf FSR0L
	xorlw kInvalidMIBIndex
	skipnz
		goto unsupported_error

	;Initialize to no error and call the slave handler
	movlw kNoErrorStatus
	call _bus_slave_setreturn
	movf FSR0L,w; load the slave handler
	goto _call_handler
	;FIXME: Have the return value of the slave handler become the bus_slave_setreturn argument
	;Function returns normally

	;Errors
	checksum_error:
	movlw kChecksumMismatchStatus
	goto _bus_slave_setreturn

	unsupported_error:
	movlw kCommandNotFoundStatus
	goto _bus_slave_setreturn
ENDFUNCTION _bus_slave_callcommand
