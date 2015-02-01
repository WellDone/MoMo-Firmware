;Name: test_push_pop
;Targets: all
;Type: application
;Description: Test to ensure that the gsm_rx_buffer push and pop 
;work correctly

#include <xc.inc>
#include "asm_macros.inc"
#include "asm_branches.inc"
#include "test_asserts.inc"
#include "symbols.h"

;Must stay in sync with source file
#define GSM_BUFFER_LENGTH 		100

global _begin_tests
global _loghex, _finish_tests, _assertv

PSECT vars,local,class=RAM,delta=1
counter: ds 1

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	asm_call_gsm_rx_clear()
	call test_pushing_easy

	;We've filled the buffer, buffer_end should be 0
	banksel _rx_buffer_end_variable
	movf 	BANKMASK(_rx_buffer_end_variable),w
	assertlw 0

	call test_pushing_full

	asm_call_gsm_rx_clear()
	call test_push_pop

	banksel _rx_buffer_len_variable
	movf 	BANKMASK(_rx_buffer_len_variable),w
	assertlw 0

	asm_call_gsm_rx_clear()
	call test_pushing_easy
	call test_popping_easy
	call assert_empty_buffer

	call test_pushing_easy
	call test_pushing_full
	call test_popping_easy
	return
ENDFUNCTION _begin_tests

BEGINFUNCTION assert_empty_buffer
	banksel _rx_buffer_len_variable
	movf 	BANKMASK(_rx_buffer_len_variable),w
	assertlw 0

	banksel _rx_buffer_start_variable
	movf 	BANKMASK(_rx_buffer_start_variable),w
	assertlw 0

	banksel _rx_buffer_end_variable
	movf 	BANKMASK(_rx_buffer_end_variable),w
	assertlw 0
	return
ENDFUNCTION assert_empty_buffer

BEGINFUNCTION test_pushing_easy
	banksel counter
	clrf BANKMASK(counter)

	loop:
		incf BANKMASK(counter),f
		movf BANKMASK(counter),w
		asm_call_gsm_rx_push()

		;Make sure length == counter
		banksel _rx_buffer_len_variable
		movf BANKMASK(_rx_buffer_len_variable),w
		movwf FSR0L
		banksel counter
		movf BANKMASK(counter),w
		call _assertv

		;Make sure the last character equals what we wrote
		asm_call_gsm_rx_peek()
		movwf FSR0L
		banksel counter
		movf BANKMASK(counter),w
		call _assertv

		banksel counter
		movf BANKMASK(counter),w
		xorlw GSM_BUFFER_LENGTH
		skipz
			goto loop
	return
ENDFUNCTION test_pushing_easy

BEGINFUNCTION test_pushing_full
	banksel counter
	clrf BANKMASK(counter)

	loop2:
		incf BANKMASK(counter),f
		movf BANKMASK(counter),w
		asm_call_gsm_rx_push()

		;Make sure length == GSM_BUFFER_LENGTH
		banksel _rx_buffer_len_variable
		movf BANKMASK(_rx_buffer_len_variable),w
		movwf FSR0L
		movlw GSM_BUFFER_LENGTH
		call _assertv

		;Make sure the last character equals what we wrote
		asm_call_gsm_rx_peek()
		movwf FSR0L
		banksel counter
		movf BANKMASK(counter),w
		call _assertv

		banksel counter
		movf BANKMASK(counter),w
		xorlw 255
		skipz
			goto loop2
	return
ENDFUNCTION test_pushing_full

BEGINFUNCTION test_popping_easy
	banksel counter
	movlw GSM_BUFFER_LENGTH
	movwf BANKMASK(counter)

	loop3:
		decf BANKMASK(counter),f
		asm_call_gsm_rx_pop()

		;Make sure the last character equals what we wrote
		movwf FSR0L
		banksel counter
		movf BANKMASK(counter),w
		sublw GSM_BUFFER_LENGTH
		call _assertv

		;Make sure length == counter
		banksel _rx_buffer_len_variable
		movf BANKMASK(_rx_buffer_len_variable),w
		movwf FSR0L
		banksel counter
		movf BANKMASK(counter),w
		call _assertv

		banksel counter
		movf BANKMASK(counter),w
		skipz
			goto loop3
	return
ENDFUNCTION test_popping_easy

BEGINFUNCTION test_push_pop
	movlw 1
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_pop()
	assertlw 1

	movlw 1
	asm_call_gsm_rx_push()
	movlw 2
	asm_call_gsm_rx_push()

	asm_call_gsm_rx_pop()
	assertlw 1
	asm_call_gsm_rx_pop()
	assertlw 2
	return
ENDFUNCTION test_push_pop
