;Name: test_bus_slave
;Targets: 12lf1822
;Type: executive
;Additional: support_i2c_slave.cmd,support_find_handler_mib.as
;Description:Test to ensure tha mib bus slave handler is working correctly.  Only test
;on pic16lf1822 because we need to know which pins are the i2c clock and data lines
;for defining the i2c master module that will drive the slave handler.

#include <xc.inc>
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"
#include "asm_locations.h"

global _begin_tests
global _loghex, _finish_tests, _assertv
global _mib_buffer, _mib_packet, _mib_state

PSECT bssdata,local,class=RAM,delta=1
slave_called:
db 1

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	;Setup sentinal value
	movlw 0
	banksel slave_called
	movwf BANKMASK(slave_called)

	movlb 0
	bsf INTCON,7		;enable GIE
	bsf INTCON,6		;enable peripheral interrupts
	movlw 10
	asm_call_bus_init()	;enable mib slave mode

	banksel slave_called
	wait_for_cmd:
	movf BANKMASK(slave_called),w
	xorlw 0xAA
	btfsc ZERO
		goto wait_for_cmd

	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _test_endpoint1
	banksel slave_called
	movlw 0xAA
	movwf BANKMASK(slave_called)
	return
ENDFUNCTION _test_endpoint1

BEGINFUNCTION _test_endpoint2
	banksel slave_called
	movlw 0xAB
	movwf BANKMASK(slave_called)
	return
ENDFUNCTION _test_endpoint2

BEGINFUNCTION _delay_cycles
	movlw 255
	delayloop:
	decfsz WREG
	goto delayloop
	return 
ENDFUNCTION _delay_cycles