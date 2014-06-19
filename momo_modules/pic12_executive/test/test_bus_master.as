;Name: test_bus_master
;Targets: 12lf1822
;Type: executive
;Additional:support_bus_master.cmd
;Description:Test to ensure tha mib bus master is working correctly. Test by
;sending an rpc and seeing if it sends correctly, i.e. the master rpc logic
;does not loop forever

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
	banksel ANSELA
	clrf BANKMASK(ANSELA)

	movlb 0
	bsf INTCON,7		;enable GIE
	bsf INTCON,6		;enable peripheral interrupts
	movlw 10
	asm_call_bus_init()	;enable mib slave mode

	asm_call_bus_master_begin_rpc()
	movlb 1
	movlw 42
	movwf BANKMASK(_mib_packet+0)
	movlw 0
	movwf BANKMASK(_mib_packet+1)
	movlw 0
	movwf BANKMASK(_mib_packet+2)
	movlw 8
	asm_call_bus_master_send_rpc()
	return
ENDFUNCTION _begin_tests