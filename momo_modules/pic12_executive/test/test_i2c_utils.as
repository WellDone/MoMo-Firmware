;Name: test_i2c_utils
;Targets: all
;Type: executive
;Description: Test to ensure that i2c utility functions
;are working correctly

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
	;Make sure that the buffer location is right and that
	;nothing else is being trampled when we set the buffer
	;pointer.
	movlw 0xAA
	movwf FSR1L
	movlw 0xBB
	movwf FSR1H

	asm_call_i2c_loadbuffer()
	movf FSR0L,w
	assertlw _mib_packet
	movf FSR0H,w
	assertlw 0
	movf FSR1L,w
	assertlw 0xAA
	movf FSR1H,w
	assertlw 0xBB

	return
ENDFUNCTION _begin_tests