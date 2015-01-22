;test_asserts.inc
;Macros for asserting different things in assembly code

global _assertv, _checkpoint, _checkpoint_value

;assertlw
;Assert that the WREG = the literal value supplied
;to the macro
assertlw MACRO literal
	movwf FSR0L
	movlw literal
	call _assertv
ENDM

checkpointl MACRO literal
	movlw literal
	call _checkpoint_value
ENDM

checkpoint MACRO
	call _checkpoint
ENDM