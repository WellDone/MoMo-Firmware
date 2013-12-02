;test_asserts.inc
;Macros for asserting different things in assembly code

global _assertv

;assertlw
;Assert that the WREG = the literal value supplied
;to the macro
assertlw MACRO literal
	movwf FSR0L
	movlw literal
	call _assertv
ENDM