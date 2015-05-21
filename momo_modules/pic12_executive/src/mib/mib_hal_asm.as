#include <xc.inc>
#include "executive.h"
#include "mib12_block.h"
#include "asm_locations.h"
#include "i2c_defines.h"
#include "asm_branches.inc"

;mib_params.asm
;Assembly routines for dealing with MIB things in an efficient way
;very quickly and with minimal code overhead

ASM_INCLUDE_GLOBALS()

PSECT text100,local,class=CODE,delta=2

;Given a word offset in the mib module definition block
;in w, return the value at that address
;NOTES: 
; - Must not change the bank
BEGINFUNCTION _get_mib_block
	movwf FSR1L
	movlw low kMIBEndpointAddress
	addwf FSR1L
	movlw 0x87
	movwf FSR1H
	movf  INDF1,w
	return
ENDFUNCTION _get_mib_block

;indirect read the mib block magic value from program memory
BEGINFUNCTION _get_magic
	movlw kMIBMagicNumberOffset
	goto  _get_mib_block		
ENDFUNCTION _get_magic
