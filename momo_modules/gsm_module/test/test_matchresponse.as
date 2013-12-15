;Name: test_matching
;Targets: all
;Type: application
;Description: Test to ensure that match_response function is working 
properly  

#include <xc.inc>
#include "asm_macros.inc"
#include "symbols.h"

global _begin_tests
global _loghex, _finish_tests, _assertv

#define _gsm_buffer 0x20
#define _buffer_len 0x42

PSECT text_unittest,local,class=CODE,delta=2
BEGINFUNCTION _begin_tests
	BANKSEL(_gsm_buffer)
	movlw 'A'
	movwf BANKMASK(_gsm_buffer+0)
	movlw 'T'
	movwf BANKMASK(_gsm_buffer+1)
	movlw 13
	movwf BANKMASK(_gsm_buffer+2)
	movlw 13
	movwf BANKMASK(_gsm_buffer+3)
	movlw 10
	movwf BANKMASK(_gsm_buffer+4)
	movlw 'O'
	movwf BANKMASK(_gsm_buffer+5)
	movlw 'K'
	movwf BANKMASK(_gsm_buffer+6)
	movlw 13
	movwf BANKMASK(_gsm_buffer+7)
	movlw 10
	movwf BANKMASK(_gsm_buffer+8)

	BANKSEL(_buffer_len)
	movlw 9
	movwf BANKMASK(_buffer_len)

	asm_call_match_okay_response()
	return
ENDFUNCTION _begin_tests