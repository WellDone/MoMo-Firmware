;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"

ASM_INCLUDE_GLOBALS()

global _bus_slave_read_callback, _bus_slave_startcommand
global _i2c_loadbuffer

PSECT text_i2c_slave,local,class=CODE,delta=2

;If we just received an address, call callback to handle either read of write
;Otherwise send or receive the next byte in the i2c_buffer
BEGINFUNCTION _i2c_slave_interrupt
	banksel SSP1STAT
	btfsc 	BANKMASK(SSP1STAT), 5
		goto 	transferdata

	;We just received our address (mask out to get just the read or write status)
	movf 	BANKMASK(SSP1BUF),w
	andlw 	0b1
	skipz
		goto _bus_slave_read_callback
		goto _bus_slave_startcommand


	;We did not receive an address, so send or receive the next byte of the i2c buffer
	transferdata:
	banksel curr_loc
	clrf FSR0H
	movf BANKMASK(curr_loc),w
	movwf FSR0L

	banksel SSP1STAT
	bcf 	BANKMASK(SSP1CON1),6			;Clear SSPOV bit always

	btfss BANKMASK(SSP1STAT),2	;0 if we're being written to, 1 if we're being read from
		call _i2c_read

	banksel SSP1STAT
	btfsc BANKMASK(SSP1STAT),2
		call _i2c_write

	banksel SSP1CON1
	bsf   BANKMASK(SSP1CON1),4 	;Release the clock
	return
ENDFUNCTION _i2c_slave_interrupt

;Increment the i2c buffer pointer unless it would 
;cause an overflow
;FIXME: Add an overflow bit so that we don't keep sending checksum errors
BEGINFUNCTION _i2c_incptr
	banksel curr_loc
	movlw 	(_mib_packet + kMIBMessageSize)
	xorwf	BANKMASK(curr_loc),w
	btfsc 	ZERO
		return 			;if buffer is full, do not increment
	incf 	BANKMASK(curr_loc),f
	return
ENDFUNCTION _i2c_incptr

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
