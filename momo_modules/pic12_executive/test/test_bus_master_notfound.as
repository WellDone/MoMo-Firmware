;Name: test_bus_master_notfound
;Type: executive
;Attach Slave:8, responder_notfound.py
;I2C Capture: S, 0x8/WA, 0x0/A, 0xa/A, 0x2a/A, 0x3/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0x0/A, 0xc9/A, RS, 0x8/RA, 0x2/A, 0xfe/N, P
;Description:Test to ensure that mib bus master is working correctly. Test by
;sending an rpc and seeing if it sends correctly and that the bus master responds
;correctly that the command was not found.

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
	movwf BANKMASK(_mib_packet+2)
	movlw 3
	movwf BANKMASK(_mib_packet+3)
	
	;Send the RPC and make sure that we get the right response
	asm_call_bus_master_send_rpc()
	assertlw 0x02
	return
ENDFUNCTION _begin_tests