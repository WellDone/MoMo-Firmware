;Name: test_no_appmodule
;Targets: all
;Type: executive
;Additional: support_no_appmodule.as
;Description: Test to ensure that the executive detects when no application
;modules are installed.

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

	;Make sure we read the magic number correctly
	asm_call_get_magic()
	assertlw 0x00

	call _test_app_endpoint

	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _test_app_endpoint
	load_packet app1ication_packet
	asm_call_find_handler()
	assertlw 255

	return
ENDFUNCTION _test_app_endpoint

define_packet app1ication_packet, 10, 0, 3
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20
