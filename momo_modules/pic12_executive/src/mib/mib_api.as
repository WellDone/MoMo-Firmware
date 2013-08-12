;mib_api.as

#include <xc.inc>
#include "constants.h"

PSECT mibapi,abs,local,class=CODE,delta=2

global _main

;Use the final high words of the mib_executive rom for api callbacks
org (kFirstApplicationRow-1)*16 + 14
;dw 2
goto _main
goto _main