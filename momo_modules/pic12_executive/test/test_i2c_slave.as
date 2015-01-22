;Name: test_i2c_slave
;Targets: 12lf1822
;Type: executive
;Additional: support_i2c_slave_receive.cmd
;Description:Test to ensure tha i2c slave handler is working correctly.  Only test
;on pic12lf1822 because we need to know which pins are the i2c clock and data lines
;for defining the i2c master module that will drive the slave handler.

#include <xc.inc>
#include "asm_macros.inc"
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"

global _begin_tests
global _loghex, _finish_tests, _assertv
global _mib_buffer, _mib_packet

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	movlb 0
	bsf INTCON,7		;enable GIE
	bsf INTCON,6		;enable peripheral interrupts
	movlw 10
	asm_call_bus_init()	;enable mib slave mode

	;delay at least 8000 cycles so that we read in the message
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles

	;Make sure the message had a valid checksum.
	;This is our basic test that the slave module is clocking in
	;bytes correctly and writing them where it should
	;Our message was 1,2,0, 0xfd
	movlw 0
	asm_call_i2c_init_buffer()
	asm_call_i2c_loadbuffer()
	movf INDF0,w
	assertlw 1

	movlw 1
	asm_call_i2c_setoffset()
	asm_call_i2c_loadbuffer()
	movf INDF0,w
	assertlw 2

	movlw 2
	asm_call_i2c_setoffset()
	asm_call_i2c_loadbuffer()
	movf INDF0,w
	assertlw 0

	movlw 3
	asm_call_i2c_setoffset()
	asm_call_i2c_loadbuffer()
	movf INDF0,w
	assertlw 0xfd

	movlw 4
	asm_call_i2c_setoffset()
	asm_call_i2c_calculate_checksum()
	assertlw 0x00
	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _delay_cycles
	movlw 255
	delayloop:
	decfsz WREG
	goto delayloop
	return 
ENDFUNCTION _delay_cycles