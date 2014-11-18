;Name: test_bus_checkparam
;Targets: all
;Type: executive
;Additional: support_no_appmodule.as
;Description:Test to ensure tha mib bus slave handler is working correctly.  Make
;sure that when we receive a mib command with arguments that we correctly calculate
;their length.

#include <xc.inc>
#include "symbols.h"
#include "test_macros.inc"
#include "test_asserts.inc"
#include "asm_locations.h"

global _begin_tests
global _loghex, _finish_tests, _assertv
global _mib_buffer, _mib_packet, _mib_state

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
;FIXME: Add checks here
	return
ENDFUNCTION _begin_tests