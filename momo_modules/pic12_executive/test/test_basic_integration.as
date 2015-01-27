;Name: test_basic_integration
;Targets: all
;Type: executive_integration
;Additional: support_valid_app.as,support_i2c_pullups.cmd
;Checkpoints: _begin_tests=0
;Description: Test to ensure that executive integration tests work
;correctly and run the entire executive, finding an application module 
;and executing it.

#include <xc.inc>
#include "asm_macros.inc"
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"

global _begin_tests
global _loghex, _finish_tests, _assertv, _checkpoint
global _mib_buffer, _mib_packet

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	checkpoint
	return
ENDFUNCTION _begin_tests