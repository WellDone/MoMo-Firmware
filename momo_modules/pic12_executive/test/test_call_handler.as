;Name: test_call_handler
;Targets: all
;Type: executive
;Description: Test to ensure that the find_handler function works  

#include <xc.inc>
#include "asm_locations.h"
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"

global _begin_tests
global _loghex, _finish_tests, _assertv
global _mib_buffer, _mib_packet, _mib_state

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	;Check for a mib executive endpoint that should exist
	load_packet mib_test
	asm_call_find_handler()
	assertlw 2
	movlb 1
	movlw 2
	movwf BANKMASK(slave_handler)
	asm_call_call_handler()

	return
ENDFUNCTION _begin_tests



define_packet mib_test, 1, 2, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20