#include <xc.inc>

#define DEFINES_ONLY
#include "buffers.h"
#include "asm_macros.inc"
#include "asm_branches.inc"

global _gsm_rx_buffer, _rx_buffer_start, _rx_buffer_end, _rx_buffer_len
global _mib_to_fsr0
global _gprs_apn, _comm_destination
global _expected1, _expected2, _expected1_ptr, _expected2_ptr
global _mib_buffer,_mib_packet

PSECT gsmvars,global,class=RAM,delta=1
_rx_buffer_start: ds 1
_rx_buffer_end: ds 1
_rx_buffer_len: ds 1

_expected1: ds 2
_expected2: ds 2

_expected1_ptr: ds 2
_expected2_ptr: ds 2

PSECT gsmbuffers,global,class=LINEAR,delta=1
_gsm_rx_buffer: 	ds GSM_RECEIVE_BUFFER_LENGTH
_gprs_apn:			ds GSM_APN_LENGTH
_comm_destination:	ds GSM_COMM_DESTINATION_LENGTH

PSECT gsmrx_text,global,class=CODE,delta=2

;Take the pointers in _expected1 or _expected2 and
;copy them over to the working variables _expected1_ptr
;or _expected2_ptr
BEGINFUNCTION _reset_expected1_ptr
	banksel _expected1
	movf BANKMASK(_expected1),w
	movwf BANKMASK(_expected1_ptr)
	movf BANKMASK(_expected1+1),w
	movwf BANKMASK(_expected1_ptr+1)
	return
ENDFUNCTION _reset_expected1_ptr

BEGINFUNCTION _reset_expected2_ptr
	banksel _expected2
	movf BANKMASK(_expected2),w
	movwf BANKMASK(_expected2_ptr)
	movf BANKMASK(_expected2+1),w
	movwf BANKMASK(_expected2_ptr+1)
	return
ENDFUNCTION _reset_expected2_ptr

;Check if the value in W is equal to the current value
;pointed to by expected1_ptr and if so, increment
;expected1_ptr and return 0 if the string terminated
;otherwise, reset _expected1_ptr and return 0xFF
;USES: FSR0
BEGINFUNCTION _check_inc_expected1
	banksel _expected1_ptr
	movwf 	FSR0H
	movf BANKMASK(_expected1_ptr),w
	movwf FSR0L
	movf BANKMASK(_expected1_ptr+1),w
	xorwf FSR0H,w
	xorwf FSR0H,f
	xorwf FSR0H,w
	xorwf INDF0,w
	skipnz 
		goto matched_exp1

	;Was not matched
	call _reset_expected1_ptr
	retlw 0xFF

	matched_exp1:
	incf BANKMASK(_expected1_ptr),f
	skipnz
		incf BANKMASK(_expected1_ptr+1),f
	moviw [1]FSR0
	return
ENDFUNCTION _check_inc_expected1

BEGINFUNCTION _check_inc_expected2
	banksel _expected2_ptr
	movwf 	FSR0H
	movf BANKMASK(_expected2_ptr),w
	movwf FSR0L
	movf BANKMASK(_expected2_ptr+1),w
	xorwf FSR0H,w
	xorwf FSR0H,f
	xorwf FSR0H,w
	xorwf INDF0,w
	skipnz 
		goto matched_exp2

	;Was not matched
	call _reset_expected2_ptr
	retlw 0xFF

	matched_exp2:
	incf BANKMASK(_expected2_ptr),f
	skipnz
		incf BANKMASK(_expected2_ptr+1),f
	moviw [1]FSR0
	return
ENDFUNCTION _check_inc_expected2

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
;Does not affect bank
BEGINFUNCTION _load_buffer
	movwf FSR1H
	call _buffer_table_low
	movwf FSR1L
	movf FSR1H,w
	call _buffer_table_high
	movwf FSR1H
	return
ENDFUNCTION _load_buffer

;Look at the last character in the gsm_rx_buffer
;RETURN: last character or 0 if there is no character
BEGINFUNCTION _gsm_rx_peek
	banksel _rx_buffer_len
	movf 	BANKMASK(_rx_buffer_len),w
	skipnz
		retlw 0

	movlw kReceiveBuffer
	call _load_buffer

	movf BANKMASK(_rx_buffer_end),w
	skipnz
		movlw GSM_RECEIVE_BUFFER_LENGTH

	decf WREG,w
	call _add_w_fsr1
	movf INDF1,w
	return
ENDFUNCTION _gsm_rx_peek

;Push a character onto the gsm_rx_buffer circular buffer
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
	call _add_w_fsr1
	incf BANKMASK(_rx_buffer_start),f
	movf BANKMASK(_rx_buffer_start),w
	xorlw GSM_RECEIVE_BUFFER_LENGTH
	skipnz
		clrf BANKMASK(_rx_buffer_start)
	movf BANKMASK(_rx_buffer_start),w
	movwf BANKMASK(_rx_buffer_end)

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

;pop the oldest value from the ringbuffer and return
;it in WREG.
;USES: FSR1
BEGINFUNCTION _gsm_rx_pop
	banksel _rx_buffer_len
	movf BANKMASK(_rx_buffer_len),w
	skipnz
		retlw 0

	decf BANKMASK(_rx_buffer_len),f

	movlw kReceiveBuffer
	call _load_buffer

	movf BANKMASK(_rx_buffer_start),w
	call _add_w_fsr1
	incf BANKMASK(_rx_buffer_start),f
	movf BANKMASK(_rx_buffer_start),w
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
	retlw LOW _gprs_apn
	retlw LOW _comm_destination
ENDFUNCTION _buffer_table_low

BEGINFUNCTION _buffer_table_high
	brw
	retlw HIGH _gsm_rx_buffer
	retlw HIGH _gprs_apn
	retlw HIGH _comm_destination
ENDFUNCTION _buffer_table_high

;Set up FSR0 to point to the mib buffer
BEGINFUNCTION _mib_to_fsr0
	movlw low _mib_buffer
	movwf FSR0L
	movlw high _mib_buffer
	movwf FSR0H
	return
ENDFUNCTION _mib_to_fsr0

;Copy WREG bytes from FSR0 to FSR1
BEGINFUNCTION _copy
	;Make sure we dont copy zero bytes
	movf WREG,w
	btfsc ZERO
	return

	copyloop:
	movwf INDF1
	moviw FSR0++
	xorwf INDF1,w 	;swap w with copied byte
	xorwf INDF1,f
	xorwf INDF1,w
	addfsr FSR1,1
	decfsz WREG,w
	goto copyloop

	return
ENDFUNCTION _copy