;Name: test_rowcalcs
;Targets: all
;Type: application
;Description: Test that the row calculations in flash_memory.as are producing
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
	movlw 32
	asm_callprepare_row_address()
	banksel (EEADRL)
	movf BANKMASK(EEADRL),w
	assertlw (32*kFlashRowSize) & 0xFF
	movf BANKMASK(EEADRH),w
	andlw (kFlashMemorySize-1)>>8
	assertlw (32*kFlashRowSize) >> 8

	movlw 127
	asm_callprepare_row_address()
	banksel (EEADRL)
	movf BANKMASK(EEADRL),w
	assertlw (127*kFlashRowSize) & 0xFF
	movf BANKMASK(EEADRH),w
	andlw (kFlashMemorySize-1)>>8
	assertlw (127*kFlashRowSize) >> 8
	return
ENDFUNCTION _begin_tests