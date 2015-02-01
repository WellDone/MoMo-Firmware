#include <xc.inc>

#define DEFINES_ONLY
#include "buffers.h"
#include "asm_macros.inc"
#include "asm_branches.inc"

global _gsm_rx_buffer, _rx_buffer_start, _rx_buffer_end, _rx_buffer_len

PSECT gsmvars,global,class=RAM,delta=1
_rx_buffer_start: ds 1
_rx_buffer_end: ds 1
_rx_buffer_len: ds 1

PSECT gsmbuffers,global,class=BIGRAM,delta=1
_gsm_rx_buffer: ds GSM_RECEIVE_BUFFER_LENGTH

PSECT gsmrx_text,global,class=CODE,delta=2

;Initialize the gsm_rx_buffer fullness variables to 0
BEGINFUNCTION _gsm_rx_clear
	banksel _rx_buffer_len
	clrf 	BANKMASK(_rx_buffer_len)
	banksel _rx_buffer_start
	clrf 	BANKMASK(_rx_buffer_start)
	banksel _rx_buffer_end
	clrf	BANKMASK(_rx_buffer_end)
	return
ENDFUNCTION _gsm_rx_clear

;Take a buffer ID number and load that buffer into FSR1
BEGINFUNCTION _load_buffer
	movwf FSR1H
	call _buffer_table_low
	movwf FSR1L
	movf FSR1H,w
	call _buffer_table_high
	movwf FSR1H
	return
ENDFUNCTION _load_buffer

;char gsm_rx_peek()
;{
;	if ( rx_buffer_len == 0 )
;		return 0;
;	if ( rx_buffer_end == 0 )
;		return gsm_rx_buffer[RX_BUFFER_LENGTH-1];
;	else
;		return gsm_rx_buffer[rx_buffer_end-1];
;}

;Look at the last character in the gsm_rx_buffer
;RETURN: last character or 0 if there is no character
BEGINFUNCTION _gsm_rx_peek
	banksel _rx_buffer_len
	movf 	BANKMASK(_rx_buffer_len),w
	skipnz
		retlw 0

	movlw kReceiveBuffer
	call _load_buffer

	banksel _rx_buffer_end
	movf BANKMASK(_rx_buffer_end),w
	skipnz
		movlw GSM_RECEIVE_BUFFER_LENGTH

	decf WREG,w
	call _add_w_fsr1
	movf INDF1,w
	return
ENDFUNCTION _gsm_rx_peek

;if ( rx_buffer_len < GSM_RECEIVE_BUFFER_LENGTH )
;{
;	gsm_rx_buffer[rx_buffer_end++] = RCREG;
;	if ( rx_buffer_end >= GSM_RECEIVE_BUFFER_LENGTH )
;		rx_buffer_end = 0;
;	++rx_buffer_len;
;}
;else
;{
;	gsm_rx_buffer[rx_buffer_start++] = RCREG;
;	rx_buffer_end = rx_buffer_start;
;	if ( rx_buffer_start >= GSM_RECEIVE_BUFFER_LENGTH )
;		rx_buffer_start = 0;
;}

;push a character onto the gsm_rx_buffer circular buffer
;PARAMETERS: one character in W
;USES:FSR0L,FSR1
BEGINFUNCTION _gsm_rx_push
	;save off byte for later
	movwf FSR0L
	
	;load the receive buffer
	movlw kReceiveBuffer
	call _load_buffer

	;Check if rx_buffer_len == GSM_RECEIVE_BUFFER_LENGTH
	banksel _rx_buffer_len
	movf BANKMASK(_rx_buffer_len),w
	xorlw GSM_RECEIVE_BUFFER_LENGTH
	skipnz 
		goto buffer_full
	goto buffer_not_full

	buffer_full:
	banksel _rx_buffer_start
	movf BANKMASK(_rx_buffer_start),w
	banksel _rx_buffer_start
	call _add_w_fsr1
	incf BANKMASK(_rx_buffer_start),f
	movf BANKMASK(_rx_buffer_start),w
	banksel _rx_buffer_end
	movwf BANKMASK(_rx_buffer_end)
	banksel _rx_buffer_start
	xorlw GSM_RECEIVE_BUFFER_LENGTH
	skipnz
		clrf BANKMASK(_rx_buffer_start)
	goto load_character


	buffer_not_full:
	banksel(_rx_buffer_end)
	movf BANKMASK(_rx_buffer_end),w
	call _add_w_fsr1
	incf BANKMASK(_rx_buffer_end),f

	;If rx_buffer_end == GSM_RECEIVE_BUFFER_LENGTH, zero it
	movlw GSM_RECEIVE_BUFFER_LENGTH
	xorwf BANKMASK(_rx_buffer_end),w
	skipnz
		clrf BANKMASK(_rx_buffer_end)

	banksel _rx_buffer_len
	incf BANKMASK(_rx_buffer_len)

	load_character:
	;Load character into buffer
	movf FSR0L,w
	movwf INDF1
	return
ENDFUNCTION _gsm_rx_push

;char gsm_rx_pop()
;{
;	if ( rx_buffer_len == 0 )
;		return 0;
;	
;	--rx_buffer_len;
;
;	char res = gsm_rx_buffer[rx_buffer_start++];
;	if ( rx_buffer_start == GSM_RECEIVE_BUFFER_LENGTH )
;		rx_buffer_start = 0;
;	return res;
;}
;Pop the oldest entry off of the gsm_rx_buffer and return it

BEGINFUNCTION _gsm_rx_pop
	banksel _rx_buffer_len
	movf BANKMASK(_rx_buffer_len),w
	skipnz
		retlw 0

	decf BANKMASK(_rx_buffer_len),f

	movlw kReceiveBuffer
	call _load_buffer

	banksel _rx_buffer_start
	movf BANKMASK(_rx_buffer_start),w
	call _add_w_fsr1
	incf BANKMASK(_rx_buffer_start),f
	xorlw GSM_RECEIVE_BUFFER_LENGTH
	skipnz
		movwf BANKMASK(_rx_buffer_start)

	movf INDF1,w
	return
ENDFUNCTION _gsm_rx_pop

BEGINFUNCTION _add_w_fsr1
	addwf FSR1L,f
	movlw 0
	addwfc FSR1H,f
	return
ENDFUNCTION _add_w_fsr1

BEGINFUNCTION _buffer_table_low
	brw
	retlw LOW _gsm_rx_buffer
ENDFUNCTION _buffer_table_low

BEGINFUNCTION _buffer_table_high
	brw
	retlw HIGH _gsm_rx_buffer
ENDFUNCTION _buffer_table_high

