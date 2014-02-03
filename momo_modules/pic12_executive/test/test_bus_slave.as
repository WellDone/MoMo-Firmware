;Name: test_bus_slave
;Targets: 12lf1822
;Type: executive
;Additional: support_i2c_slave.cmd
;Description:Test to ensure tha mib bus slave handler is working correctly.  Only test
;on pic12lf1822 because we need to know which pins are the i2c clock and data lines
;for defining the i2c master module that will drive the slave handler.

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
	movlb 0
	bsf INTCON,7		;enable GIE
	bsf INTCON,6		;enable peripheral interrupts
	movlw 10
	asm_call_bus_init()	;enable mib slave mode

	wait_for_cmd:
	movlb 1
	movlw 0xFF
	xorwf BANKMASK(slave_handler),w
	btfsc ZERO
		goto wait_for_cmd

	;The MIB packet sends (1,2,0) which should load in handler 2 for verify_application
	movf BANKMASK(slave_handler),w
	assertlw 2
ENDFUNCTION _begin_tests

BEGINFUNCTION _delay_cycles
	movlw 255
	delayloop:
	decfsz WREG
	goto delayloop
	return 
ENDFUNCTION _delay_cycles