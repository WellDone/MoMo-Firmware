	.file "/home/timburke/src/welldone/MoMo-Firmware/momo_modules/mainboard_bootloader/src/memory.c"
	.section	.bootloader.spi_transfer,code
	.align	2
	.type	_spi_transfer,@function
_spi_transfer:
	.set ___PA___,1
.L2:
	btst	_SPI1STATbits,#1
	.set ___BP___,86
	bra	nz,.L2
	ze	w0,w0
	mov	w0,_SPI1BUF
.L3:
	btst	_SPI1STATbits,#0
	.set ___BP___,86
	bra	z,.L3
	mov	_SPI1BUF,w0
	return	
	.set ___PA___,0
	.section	.bootloader.spi_send_addr,code
	.align	2
	.type	_spi_send_addr,@function
_spi_send_addr:
	.set ___PA___,1
	mov.d	w8,[w15++]
	and	w1,#15,w2
	mov	#0,w3
	sl	w2,#0,w9
	mov	#0,w8
	mul.uu	w0,#1,w2
	ior	w8,w2,w8
	ior	w9,w3,w9
	lsr	w9,#0,w0
	mov	#0,w1
	rcall	_spi_transfer
	sl	w9,#8,w2
	lsr	w8,#8,w0
	ior	w2,w0,w0
	lsr	w9,#8,w1
	rcall	_spi_transfer
	mov.b	w8,w0
	rcall	_spi_transfer
	mov.d	[--w15],w8
	return	
	.set ___PA___,0
	.section	.bootloader.configure_SPI,code
	.align	2
	.global	_configure_SPI	; export
	.type	_configure_SPI,@function
_configure_SPI:
	.set ___PA___,1
	bclr.b	_SPI1CON1bits+1,#2
	bset.b	_SPI1CON1bits,#5
	bset.b	_SPI1CON1bits,#6
	bclr.b	_SPI1STATbits,#6
	bset.b	_SPI1STATbits+1,#7
	bclr.b	_TRISBbits+1,#7
	bset.b	_TRISBbits+1,#6
	bset.b	_AD1PCFGbits+1,#6
	bclr.b	_TRISBbits+1,#5
	bclr.b	_TRISBbits+1,#4
	bset.b	_LATBbits+1,#7
	return	
	.set ___PA___,0
	.section	.bootloader.mem_read,code
	.align	2
	.global	_mem_read	; export
	.type	_mem_read,@function
_mem_read:
	.set ___PA___,1
	mov.d	w8,[w15++]
	mov.d	w10,[w15++]
	mov.d	w0,w8
	mov	w2,w10
	add	w10,w3,w11
	bclr.b	_LATBbits+1,#7
	mov.b	#3,w0
	rcall	_spi_transfer
	mov.d	w8,w0
	rcall	_spi_send_addr
	sub	w10,w11,[w15]
	.set ___BP___,9
	bra	z,.L9
	clr.b	w8
.L11:
	mov.b	w8,w0
	rcall	_spi_transfer
	mov.b	w0,[w10++]
	sub	w11,w10,[w15]
	.set ___BP___,91
	bra	nz,.L11
.L9:
	bset.b	_LATBbits+1,#7
	mov.d	[--w15],w10
	mov.d	[--w15],w8
	return	
	.set ___PA___,0
	.section	.bootloader.mem_status,code
	.align	2
	.global	_mem_status	; export
	.type	_mem_status,@function
_mem_status:
	.set ___PA___,1
	bclr.b	_LATBbits+1,#7
	mov.b	#5,w0
	rcall	_spi_transfer
	clr.b	w0
	rcall	_spi_transfer
	bset.b	_LATBbits+1,#7
	return	
	.set ___PA___,0

	.section __c30_signature, info, data
	.word 0x0001
	.word 0x0000
	.word 0x0000

; MCHP configuration words

	.set ___PA___,0
	.end
