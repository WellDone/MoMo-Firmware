.include "p24F16KA101.inc"

.global _flash_operation
.section .bootloader,code
	
_flash_operation:	;W0=NVMCON - no return values
	disi #6
	mov	W0,NVMCON
	mov	#0x55,W0	;Unlock sequence - interrupts need to be off
	mov	W0,NVMKEY
	mov	#0xAA,W0
	mov	W0,NVMKEY
	bset NVMCON,#WR
	nop				;Required
	nop
write_wait:	
	btsc NVMCON,#WR	;Wait for write end
	bra write_wait
	return
.end
