;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"

ASM_INCLUDE_GLOBALS()

global _plist_param_length,_i2c_buffersize

#define kBusMaxMessageSize 	20

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

;Returns 0 if parameters are too long or if the indicated parameter size does not match the
;passed parameters.  Returns nonzero otherwise
;Uses:FSR0L, FSR0H
BEGINFUNCTION _bus_slave_checkparamsize
	call 	_plist_param_length
	;W now contains the parameter length
	movwf FSR0L
	movlw kBusMaxMessageSize
	movwf FSR0H
	movf  FSR0L,w
	subwf FSR0H,f
	btfss CARRY		;CARRY not borrow is clear if W > maxsize
		retlw 0x0		;

	;Check if the passed parameters equal the right size
	;FSR0L contains the size of the parameters
	;the buffer should have sizeof(command_packet + sizeof(params) + 1)
	;bytes
	call _i2c_buffersize
	addlw -4		;remember we can't subtract because sublw subtracts 4 from the literal, so add -4
	xorwf FSR0L,w 	;if buffer was the right size, this should be 0
	btfss ZERO
		retlw 0x0
	retlw 0x01
ENDFUNCTION _bus_slave_checkparamsize

;Calculate the size of the return value and return it in W
BEGINFUNCTION _bus_retval_size
	banksel bus_retstatus
	movlw 0b11111
	andwf BANKMASK(bus_retstatus),w
	return
ENDFUNCTION _bus_retval_size