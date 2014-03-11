	.file "/home/timburke/src/welldone/MoMo-Firmware/momo_modules/mainboard_bootloader/src/bootloader.c"
	.section	.bootloader.write_row,code
	.align	2
	.global	_write_row	; export
	.type	_write_row,@function
_write_row:
	.set ___PA___,1
	mov	w1,w5
	clr.b	_TBLPAG
	clr	w2
	sl	w0,#5,w6
.L2:
	mul.su	w2,#3,w0
	add	w5,w0,w3
	mov.b	[w3+2],w1
	ze	w1,w1
	add	w2,w6,w4
	add	w4,w4,w4
	mov.b	[w3+1],w3
	sl	w3,#8,w3
	mov.b	[w5+w0],w0
	ze	w0,w0
	ior	w3,w0,w0
	tblwtl.w	w0, [w4]
	tblwth.w	w1, [w4]
	inc	w2,w2
	sub	w2,#31,[w15]
	.set ___BP___,96
	bra	leu,.L2
	mov	#16388,w0
	rcall	_flash_operation
	return	
	.set ___PA___,0
	.section	.bootloader.erase_row,code
	.align	2
	.global	_erase_row	; export
	.type	_erase_row,@function
_erase_row:
	.set ___PA___,1
	clr.b	_TBLPAG
	sl	w0,#6,w0
	clr	w1
	tblwtl.w	w1, [w0]
	mov	#16472,w0
	rcall	_flash_operation
	return	
	.set ___PA___,0
	.section	.bootloader.valid_instruction,code
	.align	2
	.global	_valid_instruction	; export
	.type	_valid_instruction,@function
_valid_instruction:
	.set ___PA___,1
	clr.b	_TBLPAG
	tblrdl.w	[w0], w1
	tblrdh.w	[w0], w2
	mov	#1,w0
	add	w1,#1,[w15]
	.set ___BP___,72
	bra	nz,.L6
	com.b	w2,w2
	ze	w2,w0
	neg	w0,w0
	lsr	w0,#15,w0
.L6:
	return	
	.set ___PA___,0
	.section	.bootloader.extract_reset_vector,code
	.align	2
	.global	_extract_reset_vector	; export
	.type	_extract_reset_vector,@function
_extract_reset_vector:
	.set ___PA___,1
	mov.d	w8,[w15++]
	mul.uu	w4,#0,w4
	mov.d	w4,[w1]
	mov.b	[w0],w8
	ze	w8,w8
	mov	#0,w9
	mov.d	w8,[w1]
	mov.b	[w0+1],w6
	sl	w6,#8,w6
	mov	#0,w7
	ior	w6,w8,w6
	ior	w7,w9,w7
	mov.d	w6,[w1]
	mov.b	[w0+2],w8
	ze	w8,w9
	mov	#0,w8
	ior	w6,w8,[w1++]
	ior	w7,w9,[w1--]
	mov.d	w4,[w2]
	mov.b	[w0+3],w6
	ze	w6,w6
	mov	#0,w7
	mov.d	w6,[w2]
	mov.b	[w0+4],w4
	sl	w4,#8,w4
	mov	#0,w5
	ior	w4,w6,w4
	ior	w5,w7,w5
	mov.d	w4,[w2]
	mov.b	[w0+5],w0
	ze	w0,w1
	mov	#0,w0
	ior	w4,w0,[w2++]
	ior	w5,w1,[w2--]
	mov.d	[--w15],w8
	return	
	.set ___PA___,0
	.section	.bootloader.patch_reset_vector,code
	.align	2
	.global	_patch_reset_vector	; export
	.type	_patch_reset_vector,@function
_patch_reset_vector:
	.set ___PA___,1
	mov.b	w2,[w0]
	lsr	w3,#8,w7
	sl	w3,#8,w1
	lsr	w2,#8,w6
	ior	w1,w6,w6
	mov.b	w6,[w0+1]
	lsr	w3,#0,w2
	mov.b	w2,[w0+2]
	mov.b	w4,[w0+3]
	sl	w5,#8,w1
	lsr	w4,#8,w2
	ior	w1,w2,w2
	lsr	w5,#8,w3
	mov.b	w2,[w0+4]
	lsr	w5,#0,w4
	mov.b	w4,[w0+5]
	return	
	.set ___PA___,0
	.section	.bootloader.program_application,code
	.align	2
	.global	_program_application	; export
	.type	_program_application,@function
_program_application:
	.set ___PA___,1
	lnk	#104
	mov.d	w8,[w15++]
	mov	w10,[w15++]
	mul.uu	w0,#1,w8
	sl	w8,#0,w9
	mov	#0,w8
	bset.b	_PORTBbits,#7
	bclr.b	_TRISBbits,#7
	rcall	_configure_SPI
	mov	#4000,w0
	mov	#0,w1
	mul.uu	w2,#0,w2
	rcall	___delay32
	clr	w10
.L17:
	mov	#96,w3
	mov	#-110,w2
	add	w2,w15,w2
	mov.d	w8,w0
	rcall	_mem_read
	sub	w10,#0,[w15]
	.set ___BP___,71
	bra	nz,.L12
	sub	w15,#10,w2
	sub	w15,#14,w1
	mov	#-110,w0
	add	w0,w15,w0
	rcall	_extract_reset_vector
	mul.uu	w4,#0,w4
	mov	#10800,w2
	mov	#4,w3
	mov	#-110,w0
	add	w0,w15,w0
	rcall	_patch_reset_vector
	bra	.L13
.L12:
	sub	w10,#4,[w15]
	.set ___BP___,86
	bra	nz,.L14
	mov	[w15-10],w4
	mov	[w15-8],w5
	mov	[w15-14],w2
	mov	[w15-12],w3
	mov	#-110,w0
	add	w0,w15,w0
	rcall	_patch_reset_vector
	bra	.L13
.L14:
	mov	w10,w0
	rcall	_erase_row
	mov	#-110,w1
	add	w1,w15,w1
	mov	w10,w0
	rcall	_write_row
	inc	w10,w10
	mov	#159,w0
	sub	w10,w0,[w15]
	.set ___BP___,1
	bra	gtu,.L10
	add	#96,w8
	addc	#0,w9
	bra	.L17
.L13:
	mov	w10,w0
	rcall	_erase_row
	mov	#-110,w1
	add	w1,w15,w1
	mov	w10,w0
	rcall	_write_row
	add	#96,w8
	addc	#0,w9
	inc	w10,w10
	bra	.L17
.L10:
	mov	[--w15],w10
	mov.d	[--w15],w8
	ulnk	
	return	
	.set ___PA___,0
	.section	.bootloader.goto_address,code
	.align	2
	.global	_goto_address	; export
	.type	_goto_address,@function
_goto_address:
	.set ___PA___,0
; 111 "src/bootloader.c" 1
	goto w0
	return	
	.set ___PA___,0

	.section __c30_signature, info, data
	.word 0x0001
	.word 0x0000
	.word 0x0000

; MCHP configuration words

	.set ___PA___,0
	.end
