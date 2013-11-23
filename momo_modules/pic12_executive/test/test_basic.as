#include <xc.inc>
#include "mib12_executive_symbols.h"

global _begin_tests
global _loghex, _finish_tests, _assertv

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	movlw 0xEE
	call _loghex
	return
ENDFUNCTION _end_tests