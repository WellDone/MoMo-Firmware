;asm_utils.as

#include <xc.inc>
#include "asm_macros.inc"

global _mib_data

#define _mib_buffer (_mib_data+3)

PSECT text_asm_utils,local,class=CODE,delta=2

;Load FSR0 to point to GPR memory in bank 1
;Uses: W, FSR0L, FSR0H
BEGINFUNCTION _bank1_to_fsr0
	movlw 0xA0
	movwf FSR0L
	clrf  FSR0H,f
	return
ENDFUNCTION _bank1_to_fsr0

BEGINFUNCTION _mib_to_fsr0
	movlw (_mib_buffer & 0xFF)
	movwf FSR0L
	clrf FSR0H,f
	return
ENDFUNCTION _mib_to_fsr0

;Taking in a byte count in W, copy that many bytes from FSR1
;to FSR0, incrementing the pointers during the copy
BEGINFUNCTION _copy_fsr1_to_fsr0
	copyloop:
	moviw FSR1++
	movwi FSR0++
	decfsz WREG,w
		goto copyloop
	return
ENDFUNCTION _copy_fsr1_to_fsr0