;startup.as

#include <xc.inc>


global _main,start, _reflash_executive, intlevel1

PSECT reset_vec,global,class=CODE,delta=2
start:
goto _reflash_executive

;Special config flags telling us what size executive to copy
;and where it starts
retlw 0xAB		;return the first row of the pic12_executive, magic values so we can validate the hex file
retlw 0xCD 		;return the number of rows that the pic12_executive fills

intlevel1:

PSECT powerup,global,class=CODE,delta=2

PSECT functab,global,class=CODE,delta=2

PSECT config,global,class=CODE,delta=2

PSECT idloc,global,class=CODE,delta=2

PSECT eeprom_data,global,class=CODE,delta=2