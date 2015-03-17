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
	;Make sure buffer_end and buffer_start are in the 
	;same bank since the algorithms in buffer.as require it
	movlw _rx_buffer_end_variable >> 7
	assertlw _rx_buffer_start_variable >> 7

	movlw _rx_buffer_end_variable >> 7
	assertlw _rx_buffer_len_variable >> 7

	;Check to make sure that peeking with nothing written returns 0
	asm_call_gsm_rx_clear()
	asm_call_gsm_rx_peek()
	assertlw 0
	call assert_empty_buffer

	;Check to make sure that peeking after pushing 1 thing works (as a simple case)
	asm_call_gsm_rx_clear()
	movlw 0xAA
	asm_call_gsm_rx_push()
	banksel _rx_buffer_len_variable
	movf 	BANKMASK(_rx_buffer_len_variable),w
	assertlw 1

	banksel _rx_buffer_end_variable
	movf 	BANKMASK(_rx_buffer_end_variable),w
	assertlw 1

	asm_call_gsm_rx_peek()
	assertlw 0xAA

	;Test pushing and peeking systematically
	asm_call_gsm_rx_clear()
	call test_pushing_easy

	;We've filled the buffer, buffer_end and start should be 0
	banksel _rx_buffer_end_variable
	movf 	BANKMASK(_rx_buffer_end_variable),w
	assertlw 0

	banksel _rx_buffer_start_variable
	movf 	BANKMASK(_rx_buffer_start_variable),w
	assertlw 0

	banksel _rx_buffer_len_variable
	movf 	BANKMASK(_rx_buffer_len_variable),w
	assertlw GSM_BUFFER_LENGTH

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
	call test_push_peek

	;Make sure the buffer works when we are pushing without being aligned
	asm_call_gsm_rx_clear()
	movlw 0xAA
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_push()
	call fill_buffer
	call test_popping_easy

	;Make sure the buffer has length 0 and the start and end pointers at 3
	banksel _rx_buffer_end_variable
	movf 	BANKMASK(_rx_buffer_end_variable),w
	assertlw 3

	banksel _rx_buffer_start_variable
	movf 	BANKMASK(_rx_buffer_start_variable),w
	assertlw 3

	banksel _rx_buffer_len_variable
	movf 	BANKMASK(_rx_buffer_len_variable),w
	assertlw 0

	movlw 5
	banksel 3
	asm_call_gsm_rx_push()
	banksel 3
	asm_call_gsm_rx_peek()
	assertlw 5

	movlw 6
	banksel 1
	asm_call_gsm_rx_push()
	banksel 1
	asm_call_gsm_rx_peek()
	assertlw 6

	movlw 6
	banksel 20
	asm_call_gsm_rx_push()
	banksel 20
	asm_call_gsm_rx_peek()
	assertlw 6
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

BEGINFUNCTION fill_buffer
	banksel counter
	clrf BANKMASK(counter)

	loop4:
		incf BANKMASK(counter),f
		movf BANKMASK(counter),w
		asm_call_gsm_rx_push()

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
			goto loop4
	return
ENDFUNCTION fill_buffer

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

BEGINFUNCTION test_push_peek
	movlw 50
	asm_call_gsm_rx_push()
	asm_call_gsm_rx_peek()
	assertlw 50
	return
ENDFUNCTION test_push_peek