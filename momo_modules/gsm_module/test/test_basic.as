;Name: test_basic
;Targets: all
;Type: application
;Description: Test to ensure that the basic unit testing framework is working
;and that this test can be compile, run and interpreted.  

#include <xc.inc>
#include "asm_macros.inc"
#include "symbols.h"

global _begin_tests
global _loghex, _finish_tests, _assertv

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	movlw 0xEE
	call _loghex
	return
ENDFUNCTION _begin_tests