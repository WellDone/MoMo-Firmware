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
	movlw 1
	asm_call_i2c_init_buffer()
	asm_call_i2c_loadbuffer()
	movlw 2
	movwf INDF0
	asm_call_i2c_incptr()
	asm_call_i2c_calculate_checksum()
	assertlw 0xfe
	asm_call_i2c_append_checksum()
	asm_call_i2c_decptr()
	asm_call_i2c_loadbuffer()
	movf INDF0,w
	assertlw 0xfe

	movlw 1
	asm_call_i2c_init_buffer()
	asm_call_i2c_loadbuffer()
	asm_call_i2c_incptr()
	asm_call_i2c_loadbuffer()
	movf FSR0L,w
	assertlw (_mib_packet+2)
	return
ENDFUNCTION _begin_tests