;Name: test_bus_master_empty
;Targets: all
;Type: executive
;Additional:support_bus_master.cmd
;I2C Capture: S, 0x09/WN, 0x2a/N, 0x03/N, 0x02/N, 0x0a/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0x00/N, 0xc7/N, RS, 0x09/RN, 0xFF/A, 0xFF/A, 0xFF/N, P
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
	movwf BANKMASK(_mib_packet+0)
	movlw 3
	movwf BANKMASK(_mib_packet+1)
	movlw 2
	movwf BANKMASK(_mib_packet+2)
	
	;Send the RPC and make sure that we get the right response (0xFF in this case)
	asm_call_bus_master_send_rpc()
	assertlw 0xFF
	return
ENDFUNCTION _begin_tests