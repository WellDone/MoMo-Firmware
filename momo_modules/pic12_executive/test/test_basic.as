;Name: test_basic
;Targets: all
;Type: executive
;Description: Test to ensure that the basic unit testing framework is working
;and that this test can be compile, run and interpreted.  

#include <xc.inc>
#include "asm_macros.inc"
#include "symbols.h"
#include "test_macros.inc"

global _begin_tests
global _loghex, _finish_tests, _assertv
global _mib_buffer, _mib_packet

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	movlw 0xEE
	call _loghex
	load_packet mib_test
	banksel _mib_packet
	movf BANKMASK(_mib_packet+0),w
	call _loghex
	movf BANKMASK(_mib_packet+1),w
	call _loghex
	movf BANKMASK(_mib_packet+2),w
	call _loghex

	;asm_call_find_handler()
	;call _loghex
	return
ENDFUNCTION _begin_tests

define_packet mib_test, 1, 2, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20