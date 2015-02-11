;Name: test_expected
;Targets: all
;Type: application
;Description: Test to ensure that waiting for an expected string works

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
	movlw low hellostr
	movwf BANKMASK(_expected1_variable)
	movwf FSR1L
	movlw high hellostr | 0x80
	movwf BANKMASK(_expected1_variable+1)
	movwf FSR1H

	movf INDF1,w
	assertlw 'h'

	asm_call_reset_expected1_ptr()
	movlw 'h'
	asm_call_check_inc_expected1()
	assertlw 'e'

	movlw 'e'
	asm_call_check_inc_expected1()
	assertlw 'l'

	movlw 'l'
	asm_call_check_inc_expected1()
	assertlw 'l'

	movlw 'l'
	asm_call_check_inc_expected1()
	assertlw 'o'
	
	movlw 'o'
	asm_call_check_inc_expected1()
	assertlw 0

	;Make sure we properly recover after failing to match a 
	;character
	call _test_fail_retry1
	call _test_fail_retry2
	return
ENDFUNCTION _begin_tests

BEGINFUNCTION _test_fail_retry1
	banksel _expected1_variable
	movlw low hellostr
	movwf BANKMASK(_expected1_variable)
	movlw high hellostr | 0x80
	movwf BANKMASK(_expected1_variable+1)

	asm_call_reset_expected1_ptr()
	movlw 'h'
	asm_call_check_inc_expected1()
	assertlw 'e'

	movlw 'e'
	asm_call_check_inc_expected1()
	assertlw 'l'

	;Check for failure
	movlw 'a'
	asm_call_check_inc_expected1()
	assertlw 0xFF

	;Make sure it restarted correctly
	movlw 'h'
	asm_call_check_inc_expected1()
	assertlw 'e'

	movlw 'e'
	asm_call_check_inc_expected1()
	assertlw 'l'

	movlw 'l'
	asm_call_check_inc_expected1()
	assertlw 'l'

	movlw 'l'
	asm_call_check_inc_expected1()
	assertlw 'o'
	
	movlw 'o'
	asm_call_check_inc_expected1()
	assertlw 0
	return
ENDFUNCTION _test_fail_retry1

BEGINFUNCTION _test_fail_retry2
	banksel _expected2_variable
	movlw low hellostr
	movwf BANKMASK(_expected2_variable)
	movlw high hellostr | 0x80
	movwf BANKMASK(_expected2_variable+1)

	asm_call_reset_expected2_ptr()
	movlw 'h'
	asm_call_check_inc_expected2()
	assertlw 'e'

	movlw 'e'
	asm_call_check_inc_expected2()
	assertlw 'l'

	;Check for failure
	movlw 'a'
	asm_call_check_inc_expected2()
	assertlw 0xFF

	;Make sure it restarted correctly
	movlw 'h'
	asm_call_check_inc_expected2()
	assertlw 'e'

	movlw 'e'
	asm_call_check_inc_expected2()
	assertlw 'l'

	movlw 'l'
	asm_call_check_inc_expected2()
	assertlw 'l'

	movlw 'l'
	asm_call_check_inc_expected2()
	assertlw 'o'
	
	movlw 'o'
	asm_call_check_inc_expected2()
	assertlw 0
	return
ENDFUNCTION _test_fail_retry2

PSECT strings,local,class=CODE,delta=2
hellostr: 	db 'h','e','l','l','o',0
okstr: 		db 'O','K',0