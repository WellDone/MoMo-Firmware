#include <xc.inc>
#define _DEFINES_ONLY
#include "bootloader.h"
#include "constants.h"
#undef _DEFINES_ONLY

global _flash_erase_application,_flash_write_row, _flash_erase_row
global _get_boot_source, _get_firmware_id, _get_magic,_mib_buffer
global _copy_mib_to_boot,_load_boot_address,_verify_application

global _boot_count


PSECT text_flash,local,class=CODE,delta=2

;write unlock sequence and initiate write sequence
;bank3 must be selected before calling
unlock_and_write:
	movlw	0x55					;required sequence
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

read_app_row:
	call 	prepare_row_address
	movlw 	16
	movwf 	FSR0L

	read_row_loop
	bsf  	RD						;in bank 3 from prepare_row_address
	nop
	nop

	movf  	BANKMASK(EEDATL),w
	addwf 	BANKMASK(EEDATH),w
	addwf 	FSR1H,f

	incf 	BANKMASK(EEADRL),f
	decfsz  FSR0L
	goto read_row_loop
	return

;Calculate an 8-bit checksum of all of application flash
_verify_application:
	clrf FSR1H						;store checksum here
	movlw 	kFirstApplicationRow
	movwf 	FSR1L					;current row in FSR1L
	read_app_loop:
	movf  	FSR1L,w
	call  	read_app_row
	incf 	FSR1L,f 				;current row++
	movlw	kNumFlashRows			;if row == memsize, we're done
	subwf	FSR1L,w
	btfss	ZERO
	goto 	read_app_loop

	movf    FSR1H,w
	return

;taking in a row number in W, erase that flash row
;affects EEADR{L,H}
_flash_erase_row:
    call 	prepare_row_address
    bsf		FREE 					;perform an erase on next WR command, cleared by hardware
    call	unlock_and_write
    return

;taking in no parameters, erase all of the rows corresponding to the application code
;as defined in bootloader.h
_flash_erase_application:
	bcf		GIE
	movlw 	kFirstApplicationRow
	movwf 	FSR1L					;current row in FSR1L
	erase_app_loop:
	movf  	FSR1L,w
	call  	_flash_erase_row
	incf 	FSR1L,f 				;current row++
	movlw	kNumFlashRows			;if row == memsize, we're done
	subwf	FSR1L,w
	btfss	ZERO
	goto 	erase_app_loop
	bcf		WREN 					;disallow program/erase
	bsf		GIE
	return

;take in a byte specifying a flash row number and load the 16 bit address
;of the first byte of that row into the mib_buffer in position 1
;the low order byte is assumed to contain the offset with the row so it is
;combined with a logical or.
_load_boot_address:
	movlb 0
	lslf _boot_count,w
	movlb 1
	movwf FSR1L

	andlw 0xF0
	swapf WREG
	movwf BANKMASK(_mib_buffer+3)
	
	movf  FSR1L,w
	andlw 0x0F
	swapf WREG
	iorwf BANKMASK(_mib_buffer+2),f
	return


;If the device is in bootloader mode returns the address of 
;the device to get the application code from
_get_boot_source:
	MOVLW 0x87
	MOVWF FSR1H
	movlw 0xFE
	movwf FSR1L
	movf INDF1,W
	return

;Given that a proper bootloader preparation structure is written
;in high memory, return the firmware id that we should program
_get_firmware_id:
	MOVLW 0x87
	MOVWF FSR1H
	MOVLW 0xFD
	MOVWF FSR1L
	MOVF INDF1,W
	return

;Given an offset in W, copy 16 bytes from the mib_buffer
;to the programming buffer + offset
_copy_mib_to_boot:
	movlb 3
	clrf   FSR0H
	addlw kBootloaderBufferLoc
	movwf FSR0L
	clrf   FSR1H
	movlw _mib_buffer
	movwf FSR1L
	movlw 16		;copy in byte at a time
	movwf BANKMASK(EEDATL)
	copyloop:
	moviw FSR1++ 	;copy one byte
	movwi FSR0++ 	
	decfsz BANKMASK(EEDATL)
	goto copyloop
	return

;taking in the row to write in the W register
;and given the first 32 bytes of bank0 GPR
;filled with the row we want to write, do the write.
_flash_write_row:
	bcf		GIE
	call 	prepare_row_address		;load in the row we want to write and get ready for writing flash
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
	subwf	FSR1H,w					;check if we're at the last row, will be zero
	btfsc	ZERO
	bcf		LWLO 					;actually write if this is the last row
	call 	unlock_and_write
	incf	BANKMASK(EEADRL),f 		;increment row counter to next
	decfsz	FSR1H,f
	goto 	write_row_loop
	bcf		WREN 					;disallow program/erase
	bsf		GIE
	return