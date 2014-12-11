;Name: test_reflash
;Targets: all
;Type: application
;Description: Test that the reflashing works as expected
;the correct values

#include <xc.inc>
#include "asm_macros.inc"
#include "test_macros.inc"
#include "test_asserts.inc"
#include "symbols.h"

global _begin_tests
global _loghex, _finish_tests, _assertv

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	;asm_call_reflash_executive()
	return
ENDFUNCTION _begin_tests