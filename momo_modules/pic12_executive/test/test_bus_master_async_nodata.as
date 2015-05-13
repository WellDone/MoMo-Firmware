;Name: test_bus_master_async_nodata
;Targets: all
;Type: executive_integration
;Attach Slave:8, responder_registration.py
;Attach Slave:12, responder_async.py
;Additional: support_basic_mib.mib
;Triggered Master:1400000, master_async_response_nodata.py
;I2C Capture: S, 0x8/WA, 0x14/A, 0x7f/A, 0x2a/A, 0x0/A, XX/A, 0x1/A, 0x0/A, 0x74/A, 0x65/A, 0x73/A, 0x74/A, 0x31/A, 0x32/A, 0x1/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, XX/A, RS, 0x8/RA, 0xc0/A, 0x40/N, RS, 0x8/RA, 0xc0/A, 0x40/A, 0x0/A, 0x14/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xe2/N, P, S, 0xc/WA, 0x0/A, 0xa/A, 0x2a/A, 0x3/A, 0xa/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xbf/A, RS, 0xc/RA, 0x3f/A, 0xc1/N, P, S, 0xa/WA, 0x0/A, 0x9/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xf7/A, RS, 0xa/RA, 0x40/A, 0xc0/A, 0x0/N, P
;Description:Test to make sure that the master can make an asynchronous call
;correctly with no data returned.

#include <xc.inc>
#include "asm_macros.inc"
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"
#include "protocol_defines.h"

global _begin_tests
global _loghex, _finish_tests, _assertv
global _mib_buffer, _mib_packet

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	movlw 12
	asm_call_bus_master_begin_rpc()
	banksel(_mib_packet)
	movlw 42
	movwf BANKMASK(_mib_packet+2)
	movlw 3
	movwf BANKMASK(_mib_packet+3)
	movlw 0
	movwf BANKMASK(_mib_packet+0)
	
	;Send the RPC and make sure that we get the right response even though the response is coming
	;asynchronously
	movlw 0
	asm_call_bus_master_send_rpc()
	assertlw 0x40

	banksel(_mib_packet)
	movf BANKMASK(_mib_packet + 3),w
	assertlw 0

	;Make sure to leave time for us to clock out our response
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

BEGINFUNCTION _delay_cycles
	movlw 255
	delayloop:
	decfsz WREG
	goto delayloop
	return 
ENDFUNCTION _delay_cycles