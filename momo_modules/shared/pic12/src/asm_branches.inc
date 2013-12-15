skipz MACRO
	btfss ZERO
ENDM

skipnz MACRO
	btfsc ZERO
ENDM

skipltfl MACRO r,v
	movlw v
	subwf r,w
	btfsc CARRY
ENDM

skipltfw MACRO reg
	subwf reg,w
	btfsc CARRY
ENDM

skipgefw MACRO reg
	subwf reg,w
	btfss CARRY
ENDM

skipnelf MACRO r,v
	movf r,w
	xorlw v
	btfss ZERO
ENDM

skipnewf MACRO reg
	xorwf reg,w
	btfsc ZERO
ENDM

retf MACRO reg
	movf reg,w
	return
ENDM