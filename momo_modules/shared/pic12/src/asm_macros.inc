;asm_macros.inc
;xc8 macros to simplify defining and testing asm functions

BEGINFUNCTION MACRO func
GLOBAL func
func:
ENDM

ENDFUNCTION MACRO func
GLOBAL __end_of&func
__end_of&func:
ENDM

BEGINREGION MACRO func
GLOBAL func
func:
ENDM

ENDREGION MACRO func
GLOBAL __end_of&func
__end_of&func:
ENDM