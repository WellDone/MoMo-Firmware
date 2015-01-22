;asm_utils.as

#include <xc.inc>
#include "asm_locations.h"

ASM_INCLUDE_GLOBALS()

PSECT text_asm_utils,local,class=CODE,delta=2

BEGINFUNCTION _mib_to_fsr0
	movlw (mib_buffer & 0xFF)
	movwf FSR0L
	clrf FSR0H,f
	return
ENDFUNCTION _mib_to_fsr0

;Taking in a byte count in W, copy that many bytes from FSR1
;to FSR0, incrementing the pointers during the copy.  Use the xor
;trick to avoid needing a temporary variable since the data has to
;be copied through W.
BEGINFUNCTION _copy_fsr
	;Make sure we dont copy zero bytes
	movf WREG,w
	btfsc ZERO
	return

	copyloop:
	movwf INDF0
	moviw FSR1++
	xorwf INDF0,w 	;swap w with copied byte
	xorwf INDF0,f
	xorwf INDF0,w
	addfsr FSR0,1
	decfsz WREG,w
	goto copyloop
	return
ENDFUNCTION _copy_fsr