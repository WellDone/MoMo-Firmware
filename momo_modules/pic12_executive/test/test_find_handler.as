;Name: test_find_handler
;Targets: all
;Type: executive
;Additional: support_find_handler_mib.as
;Description: Test to ensure that the find_handler function works  

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
	;Check for a mib executive endpoint that should exist
	load_packet mib_test
	asm_call_find_handler()
	assertlw 2

	;Check for a mib executive endpoint that should not exist
	load_packet nonexistant_mib
	asm_call_find_handler()
	assertlw 255

	;Test that application mib endpoints are functional
	call _test_app_endpoints

	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _test_app_endpoints
	load_packet app1
	asm_call_find_handler()
	assertlw 0
	
	load_packet app2
	asm_call_find_handler()
	assertlw 1

	load_packet app_no1
	asm_call_find_handler()
	assertlw 255

	load_packet app_no2
	asm_call_find_handler()
	assertlw 255

	return
ENDFUNCTION _test_app_endpoints

BEGINFUNCTION _test_endpoint1
	return
ENDFUNCTION _test_endpoint1


BEGINFUNCTION _test_endpoint2
	return
ENDFUNCTION _test_endpoint2


define_packet mib_test, 1, 2, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet nonexistant_mib, 255, 255, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet app1, 10, 0, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet app2, 10, 1, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet app_no1, 10, 2, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet app_no2, 11, 0, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20