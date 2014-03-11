	.file "/home/timburke/src/welldone/MoMo-Firmware/momo_modules/mainboard_bootloader/src/main.c"
	.section	.bootloader.main,code
	.align	2
	.global	_main	; export
	.type	_main,@function
_main:
	.set ___PA___,1
	mov.d	w8,[w15++]
	mov.d	w10,[w15++]
	setm	_AD1PCFG
	bset.b	_TRISBbits,#0
	mov	#6784,w0
	mov	#6,w1
	mul.uu	w2,#0,w2
	rcall	___delay32
	btst	_PORTBbits,#0
	.set ___BP___,71
	bra	nz,.L2
	mov	#27136,w0
	mov	#24,w1
	mul.uu	w2,#0,w2
	rcall	___delay32
	btst	_PORTBbits,#0
	.set ___BP___,71
	bra	nz,.L2
	bclr.b	_PORTBbits,#0
	bclr.b	_TRISBbits,#0
	mov	#3,w0
	mov	#0,w1
	mul.uu	w2,#0,w2
	rcall	_program_application
	bset.b	_TRISBbits,#0
.L2:
	mov	#256,w0
	rcall	_valid_instruction
	sub	w0,#0,[w15]
	.set ___BP___,71
	bra	z,.L3
	mov	#256,w0
	rcall	_goto_address
.L3:
	bclr.b	_PORTBbits,#0
	mov	#16960,w8
	mov	#15,w9
	mul.uu	w10,#0,w10
.L4:
	bclr.b	_TRISBbits,#0
	mov.d	w8,w0
	mov.d	w10,w2
	rcall	___delay32
	bset.b	_TRISBbits,#0
	mov.d	w8,w0
	mov.d	w10,w2
	rcall	___delay32
	bra	.L4

	.section __c30_signature, info, data
	.word 0x0001
	.word 0x0000
	.word 0x0000

; MCHP configuration words
; Configuration word @ 0x00f80010
	.section	.config_DSWDTEN, code, address(0xf80010)
__config_DSWDTEN:
	.pword	65375
; Configuration word @ 0x00f8000e
	.section	.config_BKBUG, code, address(0xf8000e)
__config_BKBUG:
	.pword	65535
; Configuration word @ 0x00f8000c
	.section	.config_MCLRE, code, address(0xf8000c)
__config_MCLRE:
	.pword	65532
; Configuration word @ 0x00f8000a
	.section	.config_FWDTEN, code, address(0xf8000a)
__config_FWDTEN:
	.pword	65407
; Configuration word @ 0x00f80008
	.section	.config_FCKSM, code, address(0xf80008)
__config_FCKSM:
	.pword	65407
; Configuration word @ 0x00f80006
	.section	.config_IESO, code, address(0xf80006)
__config_IESO:
	.pword	65400
; Configuration word @ 0x00f80004
	.section	.config_GCP, code, address(0xf80004)
__config_GCP:
	.pword	65535
; Configuration word @ 0x00f80000
	.section	.config_BSS, code, address(0xf80000)
__config_BSS:
	.pword	65535

	.set ___PA___,0
	.end
