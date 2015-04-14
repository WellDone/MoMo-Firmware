;Name: test_bus_slave
;Targets: all
;Type: executive
;Triggered Master: 500, master_call_slave.py
;Additional: support_find_handler_mib.as
;I2C Capture: S, 0xa/WA, 0x0/A, 0x9/A, 0xa/A, 0x0/A, RS, 0xa/RA, 0x40/A, 0xc0/A, 0xa/N, P 
;Description:Test to ensure tha mib bus slave handler is working correctly by calling 
;a slave endpoint on the application module and making sure that it is called correctly

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

	asm_call_initialize()
	
	movlw 10
	asm_call_bus_init()	;enable mib slave mode

	banksel slave_called
	wait_for_cmd:
	movf BANKMASK(slave_called),w
	xorlw 0xAA
	btfss ZERO
		goto wait_for_cmd

	;Give us enough time to send the response.
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles

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