#include <xc.inc>
#define _DEFINES_ONLY
#include "bootloader.h"
#include "constants.h"
#include "asm_macros.inc"
#undef _DEFINES_ONLY

global  _get_magic,_mib_buffer, _boot_count


PSECT text_flash,local,class=CODE,delta=2

;write unlock sequence and initiate write sequence
;bank3 must be selected before calling
BEGINFUNCTION unlock_and_write
	movlw	0x55					;required sequence
    movwf	BANKMASK(EECON2)
    movlw	0xAA                  	;required sequence
    movwf   BANKMASK(EECON2)
    bsf		WR   					;set WR to begin prog/erase cycle
    nop								;2 nops required in case we're erasing
    nop
    return
ENDFUNCTION unlock_and_write

;Given a flash row number in W, multiply it by the flash row size
;to convert it to an address and store it in EEADRL and EEADRH
;currently supports only 16 and 32 byte rows
BEGINFUNCTION row_index_to_address
	banksel(EEADRL)
	swapf   WREG,w      	 			;load in the row number*kFlashRowSize into the 16 bit address
    movwf   BANKMASK(EEADRL)			;4 low bits in upper nibble of low addr
    movwf   BANKMASK(EEADRH) 			;4 high bits in lower nibble of high addr
    movlw   0xF0
    andwf   BANKMASK(EEADRL),f
    movlw   0x0F
    andwf   BANKMASK(EEADRH),f

#if kFlashRowSize == 32
	lslf 	BANKMASK(EEADRL),f
	rlf 	BANKMASK(EEADRH),f
#endif
	return
ENDFUNCTION row_index_to_address

;Prepare the row address and set the bits to allow for flash access
BEGINFUNCTION prepare_row_address
	banksel(EEADRL) 				;need to access the right bank.
    bcf		CFGS					;access FLASH program, not config
    bsf		WREN 					;allow program/erase
    bsf		EEPGD 					;access program space FLASH memory
    
    goto 	row_index_to_address
ENDFUNCTION prepare_row_address

BEGINFUNCTION read_app_row
	call 	prepare_row_address
	movlw 	kFlashRowSize
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
ENDFUNCTION read_app_row

;Calculate an 8-bit checksum of all of application flash
;Uses FSR1H and FSR1L for temporary storage
BEGINFUNCTION _verify_application
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
ENDFUNCTION _verify_application

;taking in a row number in W, erase that flash row
;affects EEADR{L,H}
BEGINFUNCTION _flash_erase_row
    call 	prepare_row_address
    bsf		FREE 					;perform an erase on next WR command, cleared by hardware
    call	unlock_and_write
    return
ENDFUNCTION _flash_erase_row

;given a byte specifying a flash row number in _boot_count and load the 16 bit address
;of the first byte of that row into the mib_buffer in position 1
;the low order byte is assumed to contain the offset with the row so it is
;combined with a logical or.
BEGINFUNCTION _load_boot_address
	banksel(_boot_count)
	movf BANKMASK(_boot_count),w
	call row_index_to_address
	banksel(EEADRL)
	movf BANKMASK(EEADRL),w
	movwf FSR1L
	movf BANKMASK(EEADRH),w
	andlw (kFlashMemorySize-1) >> 8
	movwf FSR1H
	banksel(_mib_buffer)
	lslf FSR1L,f
	rlf  FSR1H,w
	movwf BANKMASK(_mib_buffer+3)
	movf  FSR1L,w
	iorwf BANKMASK(_mib_buffer+2),f
	return
ENDFUNCTION _load_boot_address


;If the device is in bootloader mode returns the address of 
;the device to get the application code from
BEGINFUNCTION _get_boot_source
	movlw 0x87
	movwf FSR1H
	movlw 0xFE
	movwf FSR1L
	movf INDF1,W
	return
ENDFUNCTION _get_boot_source

;Given that a proper bootloader preparation structure is written
;in high memory, return the firmware id that we should program
BEGINFUNCTION _get_firmware_id
	movlw 0x87
	movwf FSR1H
	movlw 0xFD
	movwf FSR1L
	movf INDF1,W
	return
ENDFUNCTION _get_firmware_id

;Given an offset in W, copy 16 bytes from the mib_buffer
;to the programming buffer + offset
BEGINFUNCTION _copy_mib_to_boot
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
ENDFUNCTION _copy_mib_to_boot

;taking in the row to write in the W register
;and given the first 32 bytes of bank0 GPR
;filled with the row we want to write, do the write.
BEGINFUNCTION _flash_write_row
	bcf		GIE
	call 	prepare_row_address		;load in the row we want to write and get ready for writing flash
	movlw	kBootloaderBufferLoc
	movwf	FSR0L
	clrf	FSR0H 					;FSR0 now points to row array
	bsf 	LWLO					;write N-1 latches only and then actually write the row on the Nth
	movlw	kFlashRowSize
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
ENDFUNCTION _flash_write_row