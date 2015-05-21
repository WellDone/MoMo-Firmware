#include <xc.inc>
#define _DEFINES_ONLY
#include "asm_macros.inc"
#include "asm_branches.inc"
#undef _DEFINES_ONLY

global _row_buffer, _write_row, _read_row, start
global _reset_device

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
;Given the row index in W
BEGINFUNCTION prepare_row_address
	banksel(EEADRL) 				;need to access the right bank.
    bcf		CFGS					;access FLASH program, not config
    bsf		WREN 					;allow program/erase
    bsf		EEPGD 					;access program space FLASH memory
    
    goto 	row_index_to_address
ENDFUNCTION prepare_row_address

;Read the given row into the row buffer.  Row index is given in W
BEGINFUNCTION _flash_read_row
	call 	prepare_row_address
	call 	_load_row_buffer		; does not modify bank
	movlw 	kFlashRowSize
	movwf 	FSR1H

	read_row_loop:
	bsf  	RD						;in bank 3 from prepare_row_address
	nop
	nop

	movf  	BANKMASK(EEDATL),w
	movwi 	FSR0++
	movf 	BANKMASK(EEDATH),w
	movwi 	FSR0++

	incf 	BANKMASK(EEADRL),f
	decfsz  FSR1H
	goto read_row_loop
	return
ENDFUNCTION _flash_read_row

;Reflash the mib12_executive using the new executive code appended to this stub
BEGINFUNCTION _reflash_executive
	call _prepare_reflash
	
	reflash_loop:
	banksel (_read_row)
;If the pic12_executive could span more than the first page, skip the mib block row
;since that will contain our code and not the pic12_executive code
	movf 	_write_row, w
	movwf	FSR1L
	incf 	_write_row, f

#if kFlashMemorySize > 2048
	movlw 	(2048/kFlashRowSize)-1
	xorwf 	_read_row,w
	skipnz 	
		incf _read_row,f
#endif

	movf 	_read_row, w	
	incf 	_read_row, f
	call 	_flash_read_row

	movf FSR1L,w
	call _flash_erase_row
	movf FSR1L,w
	call _flash_write_row

	banksel (_write_row)
	call start+2
	xorwf BANKMASK(_write_row),w
	skipz
		goto reflash_loop

	;We've successfully reflashed, invalidate ourselves
	movlw (2048/kFlashRowSize)-1
	call _flash_erase_row
	goto _reset_device
ENDFUNCTION _reflash_executive

;taking in a row number in W, erase that flash row
;affects EEADR{L,H}
BEGINFUNCTION _flash_erase_row
    call 	prepare_row_address
    bsf		FREE 					;perform an erase on next WR command, cleared by hardware
    goto	unlock_and_write
ENDFUNCTION _flash_erase_row

;Prepare to reflash the pic12_executive from this image
;Disable interrupts, setup FSR0 to point to the row_buffer
;and initialize the current read and write rows
;Load the row buffer into FSR0
BEGINFUNCTION _prepare_reflash
	bcf 	GIE 					;we're committed, no interrupts
	banksel (_read_row)
	clrf 	_write_row

	call 	start+1
	movwf 	_read_row

	goto _load_row_buffer
ENDFUNCTION _prepare_reflash

BEGINFUNCTION _load_row_buffer
	movlw	_row_buffer
	movwf	FSR0L
	clrf	FSR0H 
	return
ENDFUNCTION _load_row_buffer

;taking in the row to write in the global_variable write_row
;and given FSR0 pointing to a buffer containing the row we wish to write
;filled with the row we want to write, do the write.
;
BEGINFUNCTION _flash_write_row
	call 	prepare_row_address		;load in the row we want to write and get ready for writing flash
	call 	_load_row_buffer 		;make sure we're pointing to the right spot
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
	return
ENDFUNCTION _flash_write_row