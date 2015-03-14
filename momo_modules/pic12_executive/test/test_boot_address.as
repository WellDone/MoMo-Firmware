;Name: test_boot_address
;Targets: all
;Type: executive
;Additional: support_no_appmodule.as
;Description: Test to ensure that the row address of flash memory is calculated correctly

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
	asm_call_load_boot_address()
	movlb 1
	movf  BANKMASK(_mib_buffer+2),w
	assertlw (kFlashRowSize*44*2) & 0x00FF
	movf BANKMASK(_mib_buffer+3),w
	assertlw ((kFlashRowSize*44*2) & 0xFF00) >> 8

	return
ENDFUNCTION _begin_tests