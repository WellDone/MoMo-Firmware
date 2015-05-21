;Name: test_bootloading_16lf1847
;Targets: 16lf1847
;Type: executive_integration
;Triggered Master: 1300000, master_start_bootloading.py
;Triggered Master: 1400000, master_reset.py
;Attach Slave:8, responder_registration.py
;Attach Slave:15, responder_bootload.py
;Additional: support_bus_slave_mib.mib
;Checkpoints: _main=0
;Copy: support_bootload_16lf1847.bin, support_bootload.bin
;Description:Test that the executive can bootload new application firmware
;correctly, validate that firmware's checksum and jump into it.  The firmware
;image that is loaded is just the test_basic_integration firmware

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
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
	call _delay_cycles
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

	movlw 10
	asm_call_bus_slave_returndata()

	banksel _mib_buffer
	movlw 1
	movwf BANKMASK(_mib_buffer+0)
	movlw 2
	movwf BANKMASK(_mib_buffer+1)
	movlw 3
	movwf BANKMASK(_mib_buffer+2)
	movlw 4
	movwf BANKMASK(_mib_buffer+3)
	movlw 5
	movwf BANKMASK(_mib_buffer+4)
	movlw 6
	movwf BANKMASK(_mib_buffer+5)
	movlw 7
	movwf BANKMASK(_mib_buffer+6)
	movlw 8
	movwf BANKMASK(_mib_buffer+7)
	movlw 9
	movwf BANKMASK(_mib_buffer+8)
	movlw 10
	movwf BANKMASK(_mib_buffer+9)
	
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