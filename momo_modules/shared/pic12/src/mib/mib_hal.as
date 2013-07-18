#include <xc.inc>

;mib_params.asm
;Assembly routines for dealing with MIB things in an efficient way
;very quickly and with minimal code overhead

GLOBAL _mib_buffer,_call_handler,_get_feature,_get_command,_get_param_spec,_get_magic,_get_num_features,_plist_int_count,_plist_param_length

PSECT text100,local,class=CODE,delta=2

;Given an index into the handler table in application code, call that handler
_call_handler:
	GOTO 0x7FE				;branch to the goto in high memory that redirects to the application code's mib callback table

_get_feature:
	GOTO 0x7FB

_get_command:
	GOTO 0x7FC

_get_param_spec:
	GOTO 0x7FD

;indirect read the highest byte from program memory
_get_magic:
	MOVLW 0x87
	MOVWF FSR1H
	MOVLW 0xFF
	MOVWF FSR1L
	MOVF INDF1,W
	RETURN

_get_num_features:
	GOTO 0x7FA

_plist_int_count:
	andlw 0b01100000	; ((plist & 0b01100000) >> 5)
	swapf WREG,w
	lsrf  WREG,w

; compute ((plist_int_count(plist) << 1) + (plist & plist_buffer_mask))
; efficiiently on the pic12
_plist_param_length:
	movlb 3
	movwf BANKMASK(EEADRL)  					
	andlw 0b01100000
	movwf BANKMASK(EEADRH)	;high contains int size
	movlw 0b00011111
	andwf BANKMASK(EEADRL),w
	addwf BANKMASK(EEADRH),w
	movlb 0
	return