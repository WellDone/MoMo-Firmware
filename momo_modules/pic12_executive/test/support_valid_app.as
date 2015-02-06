;command_map.asm
;3 structures defining the features, commands and handlers that we support

#include <xc.inc>
#include "constants.h"

#define __DEFINES_ONLY__
#include "mib_definitions.h"
#undef __DEFINES_ONLY__

;Define the number of supported features and where to store the callback table pointer
;in ROM
#define kNumFeatures 			0
#define kNumDebugCommands		0

PSECT mibblock,global,class=CODE,delta=2
;Module Name (must be exactly 8 characters long)
db 		0,0,0,0,0,0,0

;Module information
retlw 	0
retlw 	0
retlw 	0

;MIB endpoint information
retlw 	kNumFeatures
goto 	mibfeatures
goto 	mibcommands
goto 	mibspecs
goto 	mibhandlers
retlw	kMIBMagicNumber

PSECT mibstructs,local,class=CONST,delta=2

mibhandlers:
BRW

mibfeatures:
BRW

mibcommands:
BRW
RETLW kNumDebugCommands

mibspecs:
BRW