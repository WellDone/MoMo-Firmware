;Name: test_bus_master_empty
;Targets: all
;Type: executive
;Additional: support_basic_mib.mib
;I2C Capture: S, 0x9/WN, 0x0/N, 0xa/N, 0x2a/N, 0x3/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0x0/N, 0xc9/N, RS, 0x9/RN, 0xff/A, 0xff/N, P
;Description:Test to ensure that mib bus master is working correctly. Test by
;sending an rpc and seeing if it sends correctly and that the bus master detects
;that no one is listening and stops reading repeatedly.

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

	movlw 9
	asm_call_bus_master_begin_rpc()
	banksel(_mib_packet)
	movlw 42
	movwf BANKMASK(_mib_packet+2)
	movlw 3
	movwf BANKMASK(_mib_packet+3)
	
	;Send the RPC and make sure that we get the right response (0xFF in this case)
	movlw 0
	asm_call_bus_master_send_rpc()
	assertlw 0xFF
	return
ENDFUNCTION _begin_tests