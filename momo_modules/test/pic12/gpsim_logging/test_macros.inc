;test_macros.as

define_packet MACRO name, feat, cmd, spec
	name:
	db feat
	db cmd
	db spec
ENDM

define_buffer MACRO b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14,b15,b16,b17,b18,b19
	db b0
	db b1
	db b2
	db b3
	db b4
	db b5
	db b6
	db b7
	db b8
	db b9
	db b10
	db b11
	db b12
	db b13
	db b14
	db b15
	db b16
	db b17
	db b18
	db b19
ENDM

load_packet MACRO symbol
	global _copy_to_mib
	movlw low(symbol)
	movwf FSR0L
	movlw high(symbol)
	movwf FSR0H
	bsf FSR0H,7
	call _copy_to_mib
ENDM