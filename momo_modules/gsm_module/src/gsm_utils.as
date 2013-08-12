;gsm_utils.as

#include <xc.inc>

GLOBAL _match_okay_response,_match_error_response,_gsm_buffer, _buffer_len

PSECT gsmutilstext,local,class=CODE,delta=2

okay_response:
retlw 13
retlw 10
retlw 'O'
retlw 'K'
retlw 13
retlw 10
retlw 0

error_response:
retlw 13
retlw 10
retlw 'E'
retlw 'R'
retlw 'R'
retlw 'O'
retlw 'R'
retlw 13
retlw 10
retlw 0

_match_okay_response:
	movlw low okay_response
	movwf FSR1L
	movlw (high okay_response) | (1<<7)
	movwf FSR1H
	movlw 6

	goto _match_response

_match_error_response:
	movlw low error_response
	movwf FSR1L
	movlw (high error_response) | (1<<7)
	movwf FSR1H
	movlw 9

	goto _match_response

;Given a string location specified in FSR1
;and its length in W, see if it matches the 
;tail end of the gsm_buffer
_match_response:
	movwf FSR0L
	BANKSEL(_buffer_len)
	movf BANKMASK(_buffer_len),w
	movwf FSR0H 					;fsr0h contains buffer len
	movf  FSR0L,w 					;w contains match length, FSR0H contains buffer len
	subwf FSR0H,f
	btfss CARRY						; W match_length > F buffer_len
	retlw 0

	movlw low _gsm_buffer
	movwf FSR0L
	movf  FSR0H,w
	addwf FSR0L,f
	clrf  FSR0H 					;FSR0 now points to gsm buffer[len - strlen(response)], FSR1 to the response

	match_loop:
	moviw FSR1++
	btfsc ZERO 						;if we reached the end of the match string, it must be a match
	retlw 1

	xorwf INDF0,w
	btfss ZERO
	retlw 0							;if they dont match return 0
	addfsr FSR0,1
	goto match_loop