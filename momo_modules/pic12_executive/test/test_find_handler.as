;Name: test_find_handler
;Targets: all
;Type: executive_integration
;Attach Slave:8, responder_registration.py
;Additional: support_bus_slave_mib.mib
;Description: Test to ensure that mib endpoints can be found successfully.
;This test calls some endpoints that should (and shouldn't) exist on the application 
;as well as some endpoints that should and shouldn't exist in the executive and makes
;sure that they all return appropriate values.

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

	load_packet app2
	asm_call_bus_slave_callhandler()
	assertlw 0x41
	
	load_packet exec_undefined
	asm_call_bus_slave_callhandler()
	assertlw 2

	load_packet exec_status
	asm_call_bus_slave_callhandler()
	assertlw 0x40
	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _test_endpoint1
	retlw 0x10
ENDFUNCTION _test_endpoint1

BEGINFUNCTION _test_endpoint2
	retlw 0x01
ENDFUNCTION _test_endpoint2

define_packet mib_test, 2560, 3, 11
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet nonexistant_mib, 0xFFFF, 3, 11
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet app2, 1235, 3, 11
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet exec_status, 4, 3, 11
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20

define_packet exec_undefined, 25, 3, 11
db 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 ,14, 15, 16, 17, 18, 19, 20
