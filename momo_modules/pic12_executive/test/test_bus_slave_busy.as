;Name: test_bus_slave_busy
;Targets: all
;Type: executive_integration
;Triggered Master: 1200000, master_call_slave.py
;Triggered Master: 1500000, master_call_slave.py
;Attach Slave:8, responder_registration.py
;Additional: support_bus_slave_mib.mib
;I2C Capture: S, 0x8/WA, 0x0/A, 0x7f/A, 0x2a/A, 0x0/A, 0x2/A, 0x1/A, 0x0/A, 0x74/A, 0x65/A, 0x73/A, 0x74/A, 0x31/A, 0x32/A, 0x1/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x30/A, RS, 0x8/RA, 0xc0/A, 0x40/N, RS, 0x8/RA, 0xc0/A, 0x40/A, 0x0/A, 0x14/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xe2/N, P, S, 0xa/WA, 0x0/A, 0x9/A, 0x0/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xed/A, RS, 0xa/RA, 0x0/A, 0x0/A, 0x0/N, P, S, 0xa/WA, 0x0/A, 0x9/A, 0x0/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xed/A, RS, 0xa/RA, 0x40/A, 0xc0/A, 0x0/N, P
;Description: Send the slave a command that does not exist and make sure that it returns
;not found.

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
	movlw 0
	banksel slave_called
	movwf BANKMASK(slave_called)

	movlw 1
	asm_call_bus_set_busy()
	banksel _mib_buffer
	movlw 0x1
	movwf BANKMASK(_mib_buffer+0)
	movlw 0x2
	movwf BANKMASK(_mib_buffer+1)
	movlw 0x3
	movwf BANKMASK(_mib_buffer+2)

	;Give us enough time to send the response.
	call _meta_delay

	;Make sure that we did not modify anything inside the mib buffer
	banksel _mib_buffer
	movf BANKMASK(_mib_buffer+0),w
	assertlw 0x1
	movf BANKMASK(_mib_buffer+1),w
	assertlw 0x2
	movf BANKMASK(_mib_buffer+2),w
	assertlw 0x3

	movlw 0
	asm_call_bus_set_busy()

	banksel slave_called
	wait_for_cmd:
	movf BANKMASK(slave_called),w
	xorlw 0xAA
	btfss ZERO
		goto wait_for_cmd

	call _meta_delay

	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _test_endpoint1
	banksel slave_called
	movlw 0xAA
	movwf BANKMASK(slave_called)
	retlw 0x00
ENDFUNCTION _test_endpoint1

BEGINFUNCTION _test_endpoint2
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