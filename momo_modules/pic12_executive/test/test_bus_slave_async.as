;Name: test_bus_slave_async
;Targets: all
;Type: executive_integration
;Triggered Master: 12000000, master_call_slave.py
;Triggered Master: 12007000, master_call_slave_notfound.py
;Triggered Master: 12500000, master_call_slave2.py
;Attach Slave:8, responder_registration.py
;Additional: support_bus_slave_mib.mib
;I2C Capture: S, 0x8/WA, 0x14/A, 0x7f/A, 0x0/A, 0x2a/A, XX/A, 0x1/A, 0x0/A, 0x74/A, 0x65/A, 0x73/A, 0x74/A, 0x31/A, 0x32/A, 0x1/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, XX/A, RS, 0x8/RA, 0xc0/A, 0x40/N, RS, 0x8/RA, 0xc0/A, 0x40/A, 0x0/A, 0x14/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xe2/N, P, S, 0xa/WA, 0x0/A, 0x9/A, 0x0/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xed/A, RS, 0xa/RA, 0x3f/A, 0xc1/A, 0x0/N, P, S, 0xa/WA, 0x0/A, 0x9/A, 0xcc/A, 0xcc/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x5f/A, RS, 0xa/RA, 0x0/A, 0x0/A, 0x0/N, P, S, 0x9/WN, 0x00/N, 0xa/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0xf6/N, RS, 0x9/RN, 0xff/A, 0xff/N, P, S, 0xa/WA, 0x0/A, 0x9/A, 0xd3/A, 0x4/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x20/A, RS, 0xa/RA, 0x40/A, 0xc0/A, 0xd3/N, P
;Description:Test to make sure that asynchronous commands to the slave are processed correctly
;In particular, send a slave command and make sure we get an async response.  Then send another slave command
;which should be responded to as busy without changing the buffers at all.  Then the slave should respond with
;a master command containing the respond to our call and finally follow up with a slave command that should go through
;because now the slave is no longer in busy/async mode

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

	banksel slave_called
	wait_for_cmd:
	movf BANKMASK(slave_called),w
	xorlw 0xAA
	btfss ZERO
		goto wait_for_cmd

	;Check to make sure the async status flag is set
	banksel (_status_variable)
	movf BANKMASK(_status_variable), w
	andlw (1<<2) | (1 << 0)
	assertlw (0b100)

	;Make sure that we respond busy after someone else sends us a command
	loop_until_next_call
	banksel (_status_variable)
	btfss BANKMASK(_status_variable), 0
		goto loop_until_next_call

	call _meta_delay
	call _meta_delay

	movlw 0
	asm_call_bus_master_async_callback()
	assertlw 0xFF

	;Make sure busy and async are no longer set since we just responded to the slave call
	banksel (_status_variable)
	movf BANKMASK(_status_variable), w
	andlw (1<<2) | (1 << 0)
	assertlw (0b000)
	
	;Make sure that we can now properly receive slave calls again
	banksel slave_called
	wait_for_cmd2:
	movf BANKMASK(slave_called),w
	xorlw 0xAB
	btfss ZERO
		goto wait_for_cmd2

	;Wait until we clock out all of the response
	call _meta_delay
	call _meta_delay

	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _test_endpoint1
	banksel slave_called
	movlw 0xAA
	movwf BANKMASK(slave_called)
	retlw 0b111111
ENDFUNCTION _test_endpoint1

BEGINFUNCTION _test_endpoint2
	banksel slave_called
	movlw 0xAB
	movwf BANKMASK(slave_called)
	retlw 0x00
ENDFUNCTION _test_endpoint2

BEGINFUNCTION _meta_delay
	movlw 255
	movwf FSR0L

	metaloop:
		call _delay_cycles
		decfsz FSR0L
		goto metaloop
	return
ENDFUNCTION _meta_delay

BEGINFUNCTION _delay_cycles
	movlw 255
	delayloop:
	decfsz WREG
	goto delayloop
	return 
ENDFUNCTION _delay_cycles