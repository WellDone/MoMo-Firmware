;utilities.as

#include <xc.inc>

global _mib_buffer, _mib_packet
global _copy_to_mib

PSECT text_unit2,local,class=CODE,delta=2

;copy_to_mib
;Given a MIB packet definition in program RAM, pointed
;to FSR0{L,R}, copy it to the mib_buffer and mib_packet
;locations

_copy_to_mib:
	movlw high(_mib_packet)
	movwf FSR1H
	movlw low(_mib_packet)
	movwf FSR1L

	movlw 3
	call _copy

	movlw high(_mib_buffer)
	movwf FSR1H
	movlw low(_mib_buffer)
	movwf FSR1L

	movlw 20
	call _copy

	return

;Copy WREG bytes from FSR0 to FSR1
_copy:
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

;Test that the first N bytes of the mib_buffer are equal to the
;first N bytes specified by FSR1
