;Name: test_flash_write
;Type: executive
;Additional: support_basic_mib.mib
;Description: Test to ensure that flash_write_row works correctly

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
	movlb 0
	movlw 44
	movwf 0x66		;boot count
	asm_call_flash_write_row()
	
	return
ENDFUNCTION _begin_tests