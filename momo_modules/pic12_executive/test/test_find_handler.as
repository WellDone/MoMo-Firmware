;Name: test_find_handler
;Targets: all
;Type: executive
;Additional: support_bus_slave_mib.mib
;Description: Test to ensure that the find_handler function works  
;FIXME Update this to test call_handler

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
	;Set the bits showing that we have a valid application loaded.
	asm_call_initialize()
	asm_call_restore_status()

	;Check for an application endpoint that should not exist
	load_packet nonexistant_mib
	asm_call_bus_slave_callhandler()
	assertlw 2

	;Check for an application endpoint that should exist
	load_packet mib_test
	asm_call_bus_slave_callhandler()
	assertlw 0x50

	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _test_endpoint1
	retlw 0x10
ENDFUNCTION _test_endpoint1

define_packet mib_test, 2560, 3, 11
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet nonexistant_mib, 0xFFFF, 3, 11
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet app2, 2651, 3, 11
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20
