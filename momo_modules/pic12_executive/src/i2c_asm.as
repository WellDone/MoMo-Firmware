;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"

ASM_INCLUDE_GLOBALS()

PSECT text_i2c_asm,local,class=CODE,delta=2

;i2c_setptr
;Given an 8-bit value in W, set the i2c starting address
;and initialize the current location to offset 0 (the start of the buffer)
;address is given relative to _mib_data
BEGINFUNCTION _i2c_init_buffer
	banksel _mib_state
	addlw 	_mib_data
	movwf 	BANKMASK(buffer_start)
	movwf 	BANKMASK(curr_loc)
	return
ENDFUNCTION _i2c_init_buffer

BEGINFUNCTION _i2c_setoffset
	banksel _mib_state
	addwf 	BANKMASK(buffer_start),w
	movwf  	BANKMASK(curr_loc)
	return
ENDFUNCTION _i2c_setoffset


;For the current i2c buffer, calculate its checksum and append the result
;to the buffer.  curr_loc will increase by one if there is space.
;Uses:FSR0,FSR1
;Returns:Nothing
BEGINFUNCTION _i2c_append_checksum
	call _i2c_calculate_checksum
	clrf FSR0H
	movwf INDF0
	goto _i2c_incptr
ENDFUNCTION _i2c_append_checksum

;Calculate the checksum of the i2c buffer from buffer_start to curr_loc-1
;Uses:FSR0 and FSR1
;Returns:Buffer checksum in W
BEGINFUNCTION _i2c_calculate_checksum
	clrf  FSR1H
	call _i2c_loadbuffer
	movf BANKMASK(buffer_start),w 		;load_buffer called banksel
	movwf FSR1L

	;FSR0L now points to 1 past the last address written
	;FSR0H will be used to sum up the checksum
	;FSR1 now points to the buffer that we wanted to check
	;If nothing has been written, return 0
	movf FSR1L,w
	xorwf FSR0L,w
	btfsc ZERO
		retlw 0x00

	validate_loop:
	moviw FSR1++
	addwf FSR0H,f
	movf FSR1L,w
	xorwf FSR0L,w
	btfss ZERO
	goto validate_loop
	comf FSR0H,f
	incf FSR0H,w
	return
ENDFUNCTION _i2c_calculate_checksum

;Load the FSR0 register with the information
;from the i2c buffer pointer.
;Postcondition: FSR0 contains address of next byte
;of i2c buffer to be sent or received.
BEGINFUNCTION _i2c_loadbuffer
	banksel _mib_state
	clrf 	FSR0H
	movf 	BANKMASK(curr_loc),w
	movwf  	FSR0L
	return
ENDFUNCTION _i2c_loadbuffer

;Return the amount of data that has been written to the buffer
BEGINFUNCTION _i2c_buffersize
	banksel _mib_state
	movf 	BANKMASK(buffer_start),w
	subwf 	BANKMASK(curr_loc),w
	return
ENDFUNCTION _i2c_buffersize

;Increment the i2c buffer pointer unless it would 
;cause an overflow
BEGINFUNCTION _i2c_incptr
	banksel _mib_state
	movlw 	(_mib_data + 24)
	xorwf	BANKMASK(curr_loc),w
	btfsc 	ZERO
		return 			;if buffer is full, do not increment
	incf 	BANKMASK(curr_loc)
	return
ENDFUNCTION _i2c_incptr

;Decrement the next available location pointer
BEGINFUNCTION _i2c_decptr
	banksel _mib_state
	decf 	BANKMASK(curr_loc)
	return
ENDFUNCTION _i2c_decptr

;Given that FSR0 contains the buffer pointer
;read 1 byte from i2c and attempt to increment
;the buffer pointer.
BEGINFUNCTION _i2c_read
	banksel SSP1BUF
	movf BANKMASK(SSP1BUF),w
	movwf INDF0
	goto _i2c_incptr
ENDFUNCTION _i2c_read

BEGINFUNCTION _i2c_write
	banksel SSP1BUF
	movf  INDF0,w
	movwf BANKMASK(SSP1BUF)
	goto _i2c_incptr
ENDFUNCTION _i2c_write