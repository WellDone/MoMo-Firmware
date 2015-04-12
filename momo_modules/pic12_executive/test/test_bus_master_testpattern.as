;Name: test_bus_master_testpattern
;Targets: all
;Type: executive
;Attach Slave:8, responder_testpattern.py
;I2C Capture: S, 0x8/WA, 0x0/A, 0xa/A, 0x0/A, 0x2a/A, 0x3/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xc9/A, RS, 0x8/RA, 0xc0/A, 0x40/N, RS, 0x8/RA, 0xc0/A, 0x40/A, 0x0/A, 0x14/A, 0x0/A, 0x1/A, 0x2/A, 0x3/A, 0x4/A, 0x5/A, 0x6/A, 0x7/A, 0x8/A, 0x9/A, 0xa/A, 0xb/A, 0xc/A, 0xd/A, 0xe/A, 0xf/A, 0x10/A, 0x11/A, 0x12/A, 0x13/A, 0x2e/N, P
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
	movwf BANKMASK(_mib_packet+3)
	movlw 3
	movwf BANKMASK(_mib_packet+4)
	
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
	movf BANKMASK(_mib_buffer+6),w
	assertlw 6
	movf BANKMASK(_mib_buffer+7),w
	assertlw 7
	movf BANKMASK(_mib_buffer+8),w
	assertlw 8
	movf BANKMASK(_mib_buffer+9),w
	assertlw 9
	movf BANKMASK(_mib_buffer+10),w
	assertlw 10
	movf BANKMASK(_mib_buffer+11),w
	assertlw 11
	movf BANKMASK(_mib_buffer+12),w
	assertlw 12
	movf BANKMASK(_mib_buffer+13),w
	assertlw 13
	movf BANKMASK(_mib_buffer+14),w
	assertlw 14
	movf BANKMASK(_mib_buffer+15),w
	assertlw 15
	movf BANKMASK(_mib_buffer+16),w
	assertlw 16
	movf BANKMASK(_mib_buffer+17),w
	assertlw 17
	movf BANKMASK(_mib_buffer+18),w
	assertlw 18
	movf BANKMASK(_mib_buffer+19),w
	assertlw 19
	return
ENDFUNCTION _begin_tests