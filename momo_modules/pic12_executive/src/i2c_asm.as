;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "protocol_defines.h"
#include "asm_locations.h"

ASM_INCLUDE_GLOBALS()

PSECT text_i2c_asm,local,class=CODE,delta=2

;For the current i2c buffer, calculate its checksum and append the result
;to the buffer.
;Uses:FSR0,FSR1
;Returns:Nothing
BEGINFUNCTION _i2c_append_checksum
	call _i2c_calculate_checksum
	movwf INDF0
	return
ENDFUNCTION _i2c_append_checksum

;Verify that the checksum of the mib buffer is valid. Returns with ZERO set
;if the checksum is valid, otherwise returns with ZERO clear
;Uses: FSR0
;Modifies: C, DC, Z
;Returns: Nothing
;Side Effets: Z set if checksum is valid, Z clear otherwise
BEGINFUNCTION _i2c_verify_checksum
	call _i2c_calculate_checksum
	xorwf INDF0,w
	return
ENDFUNCTION _i2c_verify_checksum

;Calculate the checksum of the mib buffer (packet header + data so 24 bytes)
;Uses:FSR0
;Returns:Buffer checksum in W
;Modifies:C,DC,Z
;Side Effects: None

BEGINFUNCTION _i2c_calculate_checksum
	call _i2c_loadbuffer

	;FSR0 now points to mib packet header + buffer
	;FSR0H is 0 initially because of the call to _i2c_loadbuffer
	validate_loop:
	movf FSR0H,w
	clrf FSR0H

	addwf INDF0,w
	;store W in FSR0H, which must be 0 since we're in bank1
	movwf FSR0H

	incf FSR0L,f
	movf FSR0L,w
	xorlw _mib_packet + kMIBMessageNoChecksumSize
	btfss ZERO
		goto validate_loop

	comf FSR0H,f
	incf FSR0H,w
	clrf FSR0H
	return
ENDFUNCTION _i2c_calculate_checksum

;Load the FSR0 register with the information
;from the i2c buffer pointer.
;Postcondition: FSR0 contains address
BEGINFUNCTION _i2c_loadbuffer
	clrf 	FSR0H
	movlw	_mib_packet
	movwf  	FSR0L
	return
ENDFUNCTION _i2c_loadbuffer

BEGINFUNCTION _i2c_init_location
	banksel curr_loc
	movlw 	_mib_packet
	movwf  BANKMASK(curr_loc)
	return
ENDFUNCTION _i2c_init_location
