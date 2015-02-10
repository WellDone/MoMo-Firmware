;Name: test_buffers
;Targets: all
;Type: application
;Description: Test to ensure that the asm buffers are being allocated
;as needed and the accessor functions are working correctly

#include <xc.inc>
#include "asm_macros.inc"
#include "test_asserts.inc"
#include "symbols.h"

global _begin_tests
global _loghex, _finish_tests, _assertv

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	call test_load_buffers
	call test_clear_rx
	return
ENDFUNCTION _begin_tests

BEGINFUNCTION test_load_buffers
	movlw 0
	asm_call_load_buffer()
	movf FSR1L,w
	assertlw (_gsm_rx_buffer_variable and 0xFF)
	movlw 0
	asm_call_load_buffer()
	movf FSR1H,w
	assertlw (_gsm_rx_buffer_variable shr 8)
	return
ENDFUNCTION test_load_buffers

BEGINFUNCTION test_clear_rx
	asm_call_gsm_rx_clear()
	banksel BANKMASK(_rx_buffer_len_variable)
	movf BANKMASK(_rx_buffer_len_variable),w
	assertlw 0

	banksel BANKMASK(_rx_buffer_end_variable)
	movf BANKMASK(_rx_buffer_end_variable),w
	assertlw 0

	banksel BANKMASK(_rx_buffer_start_variable)
	movf BANKMASK(_rx_buffer_start_variable),w
	assertlw 0
	return
ENDFUNCTION test_clear_rx