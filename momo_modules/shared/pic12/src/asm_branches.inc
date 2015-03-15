;Macros for testing various branching conditions in a way that's more
;transparent than what PIC's 8 bit risc architecture would lead you to
;believe is possible.

skipz MACRO
	btfss STATUS && 0x7F, 2
ENDM

skipnz MACRO
	btfsc STATUS && 0x7F, 2
ENDM

skipltfl MACRO r,v
	movlw v
	subwf r,w
	btfsc STATUS && 0x7F, 0
ENDM

skipltfw MACRO reg
	subwf reg,w
	btfsc STATUS && 0x7F, 0
ENDM

skipgefw MACRO reg
	subwf reg,w
	btfss STATUS && 0x7F, 0
ENDM

;Skip if REG != literal
;W is overwritten
skipnelf MACRO r,v
	movf r,w
	xorlw v
	btfss STATUS && 0x7F, 2
ENDM

;Skip if w != REG
skipnewf MACRO reg
	xorwf reg,w
	btfsc STATUS && 0x7F, 2
ENDM

;Load register in w and return
retf MACRO reg
	movf reg,w
	return
ENDM

;skip if W != literal
skipnel MACRO lit
	xorlw lit
	skipnz
ENDM

;skip if W == literal
skipel MACRO lit
	xorlw lit
	skipz
ENDM

;skip if W > literal
skipwgtl MACRO lit
	sublw lit
	btfss STATUS && 0x7F, 0
ENDM