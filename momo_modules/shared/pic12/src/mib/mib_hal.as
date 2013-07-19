#include <xc.inc>

;mib_params.asm
;Assembly routines for dealing with MIB things in an efficient way
;very quickly and with minimal code overhead

GLOBAL _mib_state, _mib_buffer,_call_handler,_get_feature,_get_command, _bus_slave_seterror
GLOBAL _validate_param_spec,_get_magic,_get_num_features,_plist_int_count,_plist_param_length

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
	swapf WREG,w
	movwf BANKMASK(EEDATL)	;contains int size
	movlw 0b00011111
	andwf BANKMASK(EEADRL),w
	addwf BANKMASK(EEDATL),w
	movlb 0
	return

; given the handler index in W, validate that the passed params match the spec
_validate_param_spec:
	call _get_param_spec		;handler spec in w
	movlb 1
	xorwf BANKMASK(_mib_state+3),w ;passed spec xor handler spec
	andlw 0b11100000 				;only look at spec, ignore length
	btfsc ZERO						;if they match w should be zero
	retlw 1 
	retlw 0

;void bus_slave_seterror(uint8 error)
;{
;	mib_state.bus_returnstatus.return_status = 0;
;	mib_state.bus_returnstatus.return_status |= (error << 5);
;
;	set_slave_state(kMIBProtocolError);  kMIBProtocolError is 3
;}
;bus_returnstatus is at mib_state+7
_bus_slave_seterror:
	movlb 1
	clrf BANKMASK(_mib_state+7)
	swapf WREG,w  					;error << 4
	lslf WREG,w 					; error << 1 
	iorwf BANKMASK(_mib_state+7),f 	;return_status |= error << 5
	bsf BANKMASK(_mib_state+9), 2 	;we want bits 2 and 3 set.  (set slave_state to 3)
	bsf BANKMASK(_mib_state+9), 3
	return
