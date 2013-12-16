;mib_api.as

#include <xc.inc>
#include "constants.h"
#include "asm_macros.inc"

PSECT mibapi,abs,local,class=CODE,delta=2

global _main

;Use the final high words of the mib_executive rom for api callbacks
org (kFirstApplicationRow-1)*16 + 14
;dw 2
BEGINREGION mib12_api
	goto _main
	goto _main
ENDREGION mib12_api