;Name: test_bus_master_testpattern
;Targets: all
;Type: executive
;Attach Slave:8, responder_testpattern.py
;I2C Capture: S, 0x08/WA, 0x2a/A, 0x03/A, 0x02/A, 0x0a/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0x00/A, 0xc7/A, RS, 0x08/RA, 0xc0/A, 0x40/A, 0x00/A, 0x14/A, 0x00/A, 0x01/A, 0x02/A, 0x03/A, 0x04/A, 0x05/A, 0x06/A, 0x07/A, 0x08/A, 0x09/A, 0xa/A, 0xb/A, 0xc/A, 0xd/A, 0xe/A, 0xf/A, 0x10/A, 0x11/A, 0x12/A, 0x13/A, 0x2e/N, P
;Description:Test to ensure that mib bus master is working correctly. Test by
;sending an rpc and ensuring that the response is the appropriate test pattern.

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
	asm_call_initialize()

	movlw 10
	asm_call_bus_init()	;enable mib slave mode

	movlw 8
	asm_call_bus_master_begin_rpc()
	banksel(_mib_packet)
	movlw 42
	movwf BANKMASK(_mib_packet+0)
	movlw 3
	movwf BANKMASK(_mib_packet+1)
	movlw 2
	movwf BANKMASK(_mib_packet+2)
	
	;Send the RPC and make sure that we get the right response
	asm_call_bus_master_send_rpc()
	assertlw 0b11000000

	banksel(_mib_buffer)
	movf BANKMASK(_mib_buffer+0),w
	assertlw 0
	movf BANKMASK(_mib_buffer+1),w
	assertlw 1
	movf BANKMASK(_mib_buffer+2),w
	assertlw 2
	movf BANKMASK(_mib_buffer+3),w
	assertlw 3
	movf BANKMASK(_mib_buffer+4),w
	assertlw 4
	movf BANKMASK(_mib_buffer+5),w
	assertlw 5
	return
ENDFUNCTION _begin_tests