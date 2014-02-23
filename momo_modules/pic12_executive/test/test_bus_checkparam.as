;Name: test_bus_checkparam
;Targets: all
;Type: executive
;Additional: support_no_appmodule.as
;Description:Test to ensure tha mib bus slave handler is working correctly.  Make
;sure that when we receive a mib command with arguments that we correctly calculate
;their length.

#include <xc.inc>
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"
#include "asm_locations.h"

global _begin_tests
global _loghex, _finish_tests, _assertv
global _mib_buffer, _mib_packet, _mib_state

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	movlw 0
	asm_call_i2c_init_buffer()	;enable mib slave mode
	movlw 3+4+1					;simulate receiving a mib_command + 2 parameter ints + checksum
	asm_call_i2c_setoffset()
	asm_call_i2c_buffersize()
	assertlw (3+4+1)

	movlw (2 << 5)
	banksel _mib_packet
	movwf BANKMASK(_mib_packet+2)
	asm_call_bus_slave_checkparamsize()
	assertlw 1

ENDFUNCTION _begin_tests