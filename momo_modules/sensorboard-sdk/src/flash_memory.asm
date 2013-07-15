#include <xc.inc>
#include "bootloader.h"

global _flash_erase_application,_flash_write_row


PSECT text_flash,local,class=CODE,delta=2

;write unlock sequence and initiate write sequence
;bank3 must be selected before calling
unlock_and_write:
	movlw	0x55a					;required sequence
    movwf	BANKMASK(EECON2)
    movlw	0xAA                  	;required sequence
    movwf   BANKMASK(EECON2)
    bsf		WR   					;set WR to begin prog/erase cycle
    nop								;2 nops required in case we're erasing
    nop
    return

prepare_row_address:
	movlb   3                       ;select bank3 where EEADR{L,H} live
    swapf   WREG,w                  ;load in the row number*16 into the 16 bit addres
    movwf   BANKMASK(EEADRL)		;4 low bits in upper nibble of low addr
    movwf   BANKMASK(EEADRH)		;4 high bits in lower nibble of high addr
    movlw   0xF0
    andwf   BANKMASK(EEADRL),f
    movlw   0x0F
    andwf   BANKMASK(EEADRH),f
    bcf		CFGS					;access FLASH program, not config
    bsf		WREN 					;allow program/erase
    bsf		EEPGD 					;access program space FLASH memory
    return

;taking in a row number in W, erase that flash row
;affects EEADR{L,H}
flash_erase_row:
    call 	prepare_row_address
    bsf		FREE 					;perform an erase on next WR command, cleared by hardware
    call	unlock_and_write
    return

;taking in no parameters, erase all of the rows corresponding to the application code
;as defined in bootloader.h
_flash_erase_application:
	bcf		GIE
	movlw 	kFirstApplicationRow
	movwf 	FSR1L					;base in FSR1L
	movlw 	kNumFlashRows-kFirstApplicationRow
	movwf 	FSR1H					;count in FSR1H
	erase_app_loop:
	decfsz 	FSR1H
	goto	erase_app_done
	movf  	FSR1L,w
	addwf 	FSR1H,w
	call  	flash_erase_row
	goto 	erase_app_loop
	erase_app_done:
	bcf		WREN 					;disallow program/erase
	bsf		GIE
	return

;taking in the row to write in the W register
;and given the first 32 bytes of bank0 GPR
;filled with the row we want to write, do the write.
_flash_write_row:
	bcf		GIE
	call 	prepare_row_address		;load in the row we want to write and get ready for writing flash
	movwf	FSR1L					;cache row to write in FSR1L
	movlw	kBootloaderBufferLoc
	movwf	FSR0L
	clrf	FSR0H 					;FSR0 now points to row array
	bsf 	LWLO					;write 15 latches only and then actually write the row on the 16th
	movlw	16
	movwf	FSR1H					;FSR1H has count of latches
	write_row_loop:
	moviw 	FSR0++
	movwf 	BANKMASK(EEDATL)
	moviw	FSR0++
	movwf	BANKMASK(EEDATH)
	movlw	1
	subwf	FSR1H					;check if we're at the last row, will be zero
	btfsc	ZERO
	bcf		LWLO 					;actually write if this is the last row
	call 	unlock_and_write
	incf	BANKMASK(EEADRL),f 		;increment row counter to next
	decfsz	FSR1H
	goto 	write_row_loop
	bcf		WREN 					;disallow program/erase
	bsf		GIE
	return