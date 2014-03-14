.include "p24F16KA101.inc"

.global _flash_operation, ___delay32
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
	return

;;
;; ___delay32
;;
;; 32-bit cycle delay
;;
;; inputs:
;;   w1:w0 = delay in cycles (min == 12, including call)
;; outputs:
;;   none
;; registers used:
;;   w0,w1
;;
___delay32:
        sub  #1023,w0
        subb #0,w1
        bra  lt,2f

4:      repeat #1006
        nop

        sub  #1012, w0
        subb #0, w1
        bra  ge, 4b
        add  #1,w0
2:      add  #1010,w0
        bra  lt,3f
        repeat w0
        nop
3:      return

.end

