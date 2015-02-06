;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"
#include "bus_defines.h"

ASM_INCLUDE_GLOBALS()

global _plist_param_length,_i2c_buffersize,_i2c_calculate_checksum
global _find_handler, _call_handler, _validate_param_spec

PSECT text_bus_slave,local,class=CODE,delta=2

;Given the returnvalue in WREG
;set the bus slave return status to that value
;Uses:WREG
;Returns:Nothing
BEGINFUNCTION _bus_slave_setreturn
	banksel bus_retstatus
	movwf 	BANKMASK(bus_retstatus)
	return
ENDFUNCTION _bus_slave_setreturn

;Check if parameters are too long or if the indicated parameter size does not match the
;passed parameters.   
;Uses:FSR0L, FSR0H
;Returns: Nothing
;Side Effects: 	Zero flag is set if the parameters are the right size
;				Zero flag is clear otherwise
BEGINFUNCTION _bus_slave_checkparamsize
	call 	_plist_param_length
	movwf 	FSR0L
	;W now contains the parameter length
	addlw -kBusMaxMessageSize
	btfsc CARRY		;CARRY not borrow is set if W > maxsize
		return 		;If W - maxsize < 0, i.e. !=0, the subwf will clear the zero flag

	;Check if the passed parameters equal the right size
	;FSR0L contains the size of the parameters
	;the buffer should have sizeof(command_packet + sizeof(params) + 1)
	;bytes
	call _i2c_buffersize
	addlw -4		;remember we can't subtract because sublw subtracts 4 from the literal, so add -4
	xorwf FSR0L,w 	;if buffer was the right size, this should be 0
	return 			;Zero flag set if it was the right size
ENDFUNCTION _bus_slave_checkparamsize

;Calculate the size of the return value and return it in W
BEGINFUNCTION _bus_retval_size
	banksel bus_retstatus
	movlw 0b11111
	andwf BANKMASK(bus_retstatus),w
	return
ENDFUNCTION _bus_retval_size

BEGINFUNCTION _bus_slave_callcommand
	call _i2c_calculate_checksum
	skipz
		goto checksum_error

	call _bus_slave_checkparamsize
	skipz
		goto size_error

	call _find_handler
	banksel slave_handler
	movwf BANKMASK(slave_handler)
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

	size_error:
	movlw pack_return_status(kParameterTooLong,0)
	goto _bus_slave_setreturn

	unsupported_error:
	movlw pack_return_status(kUnsupportedCommand,0)
	goto _bus_slave_setreturn

	wrongparameter_error:
	movlw pack_return_status(kWrongParameterType, 0)
	goto _bus_slave_setreturn
ENDFUNCTION _bus_slave_callcommand