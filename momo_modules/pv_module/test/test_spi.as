;Name: test_spi
;Targets: all
;Type: application
;Description: Test to ensure that the basic SPI peripheral code
 is functioning properly  

#include <xc.inc>
#include "asm_macros.inc"
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"

global _begin_tests

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	asm_call_sd_initialize()
	assertlw 0x00
	return
ENDFUNCTION _begin_tests