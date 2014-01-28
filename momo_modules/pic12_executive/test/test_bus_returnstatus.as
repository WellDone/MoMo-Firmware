;Name: test_bus_returnstatus
;Targets: all
;Type: executive
;Description:Test to ensure that i2c_append_checksum works and that
;bus_slave_setreturn works correctly
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
	movlw 4
	asm_call_bus_slave_setreturn()

	movlw 1
	asm_call_i2c_init_buffer()
	asm_call_i2c_incptr()
	asm_call_i2c_append_checksum()
	asm_call_i2c_calculate_checksum()
	assertlw 0
	return
ENDFUNCTION _begin_tests