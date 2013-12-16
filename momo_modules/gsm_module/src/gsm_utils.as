;gsm_utils.as

#include <xc.inc>
#define __DEFINES_ONLY__
#include "gsm_strings.h"
#include "asm_macros.inc"

GLOBAL _match_okay_response,_match_error_response,_gsm_buffer, _buffer_len
GLOBAL _load_gsm_constant, _match_newmsg, _match_newmsg2digit

PSECT gsmutilstext,local,class=CODE,delta=2

strtable:
brw 
retlw low s_error_response
retlw high s_error_response
retlw low s_okay_response
retlw high s_okay_response
retlw low s_start_stream
retlw high s_start_stream
retlw low s_newmsg_response
retlw high s_newmsg_response

s_start_stream:
retlw 9
db 'A','T','+','C','M','G','S','=','"',0

s_okay_response:
retlw 6			;length
retlw 13
retlw 10
retlw 'O'
retlw 'K'
retlw 13
retlw 10
retlw 0

s_newmsg_response:
db 17, 13, 10, '+', 'C','M','T','I',':',' ','"','S','M','"',',',0, 13, 10, 0

s_error_response:
retlw 9
db 13,10,'E','R','R','O','R',13,10,0

;given an index into the string table
;load the string into FSR1 and return the length
;of the string in W
load_string:
	lslf WREG,w 		;w*2
	movwf FSR0L
	call strtable
	movwf FSR1L
	incf FSR0L,w
	call strtable
	movwf FSR1H
    bsf FSR1H,7
	moviw FSR1++		;length is stored in first position
	return

;given an index in W, load the string specified into the GSM buffer
;updating the buffer_len
_load_gsm_constant:
	call load_string
	BANKSEL(_buffer_len)
	movwf BANKMASK(_buffer_len)
	movlw low _gsm_buffer
	movwf FSR0L
	clrf  FSR0H 

	copyloop:
	moviw FSR1++
	btfsc ZERO
	return
	movwi FSR0++
	goto copyloop

BEGINFUNCTION _match_okay_response
	movlw kOkayString
	call load_string
	goto _match_response
ENDFUNCTION _match_okay_response


_match_error_response:
	movlw kErrorString
	call load_string

	goto _match_response

_match_newmsg:
	movlw kNewMessageString
	call load_string
	goto _match_response

_match_newmsg2digit:
	movlw kNewMessageString
	call load_string
	movlw 18
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

	doloop:
	moviw FSR1++
	btfsc ZERO 						;if we reached the end of the match string, it must be a match
	retlw 1

	xorwf INDF0,w
	btfss ZERO
	retlw 0							;if they dont match return 0
	incf FSR0L,f
	goto doloop

	retlw 0