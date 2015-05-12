;Name: test_bus_slave_checksum1x
;Targets: all
;Type: executive_integration
;Triggered Master: 1500000, master_checksum_1x.py
;Attach Slave:8, responder_registration.py
;Additional: support_bus_slave_mib.mib
;I2C Capture: S, 0x8/WA, 0x0/A, 0x7f/A, 0x2a/A, 0x0/A, XX/A, 0x1/A, 0x0/A, 0x74/A, 0x65/A, 0x73/A, 0x74/A, 0x31/A, 0x32/A, 0x1/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, XX/A, RS, 0x8/RA, 0xc0/A, 0x40/N, RS, 0x8/RA, 0xc0/A, 0x40/A, 0x0/A, 0x14/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xe2/N, P, S, 0xa/WA, 0x0/A, 0x9/A, 0x0/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xf2/A, RS, 0xa/RA, 0x1/A, 0xff/A, 0x0/N, P, S, 0xa/WA, 0x0/A, 0x9/A, 0x0/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xed/A, RS, 0xa/RA, 0x40/A, 0xc0/A, 0x0/N, P
;Description:Test to make sure that the slave properly responds to and recovers from
;transient checksum errors in reception.

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

	;Give us enough time to send the response.
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
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
	retlw 0x00
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