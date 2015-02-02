;Name: test_gsm_check
;Targets: all
;Type: application
;Description: Test to ensure that the gsm_check function is working

#include <xc.inc>
#include "asm_macros.inc"
#include "test_asserts.inc"
#include "symbols.h"

global _begin_tests
global _loghex, _finish_tests, _assertv

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	asm_call_gsm_rx_clear()
	
	;Make sure accessing program flash works
	banksel _expected1_variable
	movlw low okstr
	movwf BANKMASK(_expected1_variable)
	movlw high okstr | 0x80
	movwf BANKMASK(_expected1_variable+1)

	banksel _expected2_variable
	movlw low errorstr
	movwf BANKMASK(_expected2_variable)
	movlw high errorstr | 0x80
	movwf BANKMASK(_expected2_variable+1)

	asm_call_reset_expected1_ptr()
	asm_call_reset_expected2_ptr()

	;Test matching the first expected value
	movlw 'H'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	movlw 'E'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	movlw 50
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'E'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'R'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'R'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0

	movlw 'O'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'K'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 1

	;Now check matching expected 2
	asm_call_reset_expected1_ptr()
	asm_call_reset_expected2_ptr()

	movlw 'H'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	movlw 'E'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	movlw 50
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'E'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'R'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'O'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0

	movlw 'O'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0

	movlw 'E'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'R'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0
	
	movlw 'R'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0

	movlw 'O'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 0

	movlw 'R'
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	asm_call_gsm_check()
	assertlw 2
	
	;check to make sure that gsm_expect_ok_error sets up variables correctly
	asm_call_gsm_expect_ok_error()
	asm_call_reset_expected1_ptr()
	asm_call_reset_expected2_ptr()
	movlw 'O'
	asm_call_gsm_check()
	assertlw 0
	movlw 'O'
	asm_call_gsm_check()
	assertlw 0
	movlw 'K'
	asm_call_gsm_check()
	assertlw 1

	asm_call_reset_expected1_ptr()
	asm_call_reset_expected2_ptr()
	movlw 'O'
	asm_call_gsm_check()
	assertlw 0
	movlw 'E'				;Give a repeated character so that we can check that error case
	asm_call_gsm_check()
	assertlw 0
	movlw 'E'
	asm_call_gsm_check()
	assertlw 0
	movlw 'R'
	asm_call_gsm_check()
	assertlw 0
	movlw 'R'
	asm_call_gsm_check()
	assertlw 0
	movlw 'O'
	asm_call_gsm_check()
	assertlw 0
	movlw 'R'
	asm_call_gsm_check()
	assertlw 2
	return
ENDFUNCTION _begin_tests

PSECT strings,local,class=CODE,delta=2
errorstr: 	db 'E','R','R','O','R',0
okstr: 		db 'O','K',0