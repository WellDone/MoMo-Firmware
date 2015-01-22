;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"

ASM_INCLUDE_GLOBALS()

global _bus_slave_read_callback, _bus_slave_startcommand
global _i2c_loadbuffer, _i2c_incptr, _i2c_read, _i2c_write

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
	call _i2c_loadbuffer
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