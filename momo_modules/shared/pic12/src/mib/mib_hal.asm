#include <xc.inc>

;mib_params.asm
;Assembly routines for creating rpc parameter specs in the mib_buffer
;very quickly and with minimal code overhead

;signature loadparams(spec)
GLOBAL _loadparams,_mib_buffer,_call_handler,_get_feature,_get_command,_get_spec,_get_magic,_get_num_features,_validate_params

PSECT text100,local,class=CODE,delta=2

;Write the current 4 bytes pointed to by FSR0 as MIBIntParameter
_loadint:
	CLRW
	MOVWI	FSR0++
	MOVLW	2	
	MOVWI	FSR0++		
	CLRW
	MOVWI	FSR0++
	MOVWI	FSR0++
	RETURN

;Write a buffer parameter into the space pointed to by FSR0
;the length is not used
_loadbuf:
	MOVLW	1
	MOVWI	FSR0++
	CLRW	 			;make it a zero length buffer, we can fill it in later	
	MOVWI	FSR0++		;FSR0 now points to the first byte of the buffer
	RETURN

;give a parameter spec in W, which is just 3 bits of count followed by bits of 0 or 1
;which tell whether to create an int or a buffer in the space
;returns the length of the parameters in w, not accounting for the length of the buffer
_loadparams:
	MOVLB 1					;mib_buffer and mib_firstfree are in bank1
	MOVWF FSR1H				;save tmp spec
	LSRF  FSR1H,f			;>>1
	LSRF  FSR1H,f			;>>2 (1 more shift and we have the param spec)
	ANDLW 0b111 			;extract only the count
	ADDLW 1                 ;increment count by 1 since we're going to decrement and then compare
	MOVWF FSR1L				;save count to SFR1L
	MOVLW _mib_buffer		;set up SFR to point to mib_buffer
	MOVWF FSR0L
	CLRF  FSR0H

loop: 
	DECFSZ FSR1L
	GOTO loopbody
	MOVLW _mib_buffer
	SUBWF FSR0L,w 			;w now stores ptr - mib_buffer which is the length of the parameter list
	RETURN

loopbody:
	LSRF FSR1H,f 			;shift to get the next param type
	BTFSS FSR1H,0
	CALL _loadint
	BTFSC FSR1H,0
	CALL _loadbuf
	GOTO loop

;Given an index into the handler table in application code, call that handler
_call_handler:
	GOTO 0x7FE				;branch to the goto in high memory that redirects to the application code's mib callback table

_get_feature:
	GOTO 0x7FB

_get_command:
	GOTO 0x7FC

_get_spec:
	GOTO 0x7FD

;indirect read the highest byte from program memory
_get_magic:
	MOVLW 0x87
	MOVWF FSR1H
	MOVLW 0xFF
	MOVWF FSR1L
	MOVF INDF1,W
	RETURN

;validate the parameters pointed to by the param spec created from the
;current slave handler index
_validate_params:
	RETLW 1

_get_num_features:
	GOTO 0x7FA

	