;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"
#include "bus_defines.h"

ASM_INCLUDE_GLOBALS()

global _plist_param_length,_i2c_calculate_checksum
global _find_handler, _call_handler, _validate_param_spec

PSECT text_bus_slave,local,class=CODE,delta=2

;Given the returnvalue in WREG set the bus slave return status to that value
;and update the bus status checksum to the correct checksum value.
;Uses:WREG
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

;Return the bus return value length 
BEGINFUNCTION _bus_retval_size
	banksel bus_length
	movf BANKMASK(bus_length),w
	return
ENDFUNCTION _bus_retval_size

BEGINFUNCTION _bus_slave_callcommand
	call _i2c_calculate_checksum
	skipz
		goto checksum_error

	call _find_handler
;	banksel slave_handler  			FIXME
;	movwf BANKMASK(slave_handler)
	movwf FSR0L						;FSR1L is the only register used by validate param spec
	xorlw kInvalidMIBIndex
	skipnz
		goto unsupported_error

	;Now validate the parameter signature
	;and if it checks out call the command since
	;its known now to be valid
	movf FSR0L,w 				;load the slave handler
	call _validate_param_spec
	skipz
		goto wrongparameter_error

	;Initialize to no error and call the slave handler
	movlw pack_return_status(kNoMIBError,0)
	call _bus_slave_setreturn
	movf FSR0L,w; load the slave handler
	goto _call_handler
	;Function returns normally

	;Errors
	checksum_error:
	movlw pack_return_status(kChecksumError, 0)
	goto _bus_slave_setreturn

	unsupported_error:
	movlw pack_return_status(kUnsupportedCommand,0)
	goto _bus_slave_setreturn

	wrongparameter_error:
	movlw pack_return_status(kWrongParameterType, 0)
	goto _bus_slave_setreturn
ENDFUNCTION _bus_slave_callcommand