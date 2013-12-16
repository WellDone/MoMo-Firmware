;command_map.asm
;3 structures defining the features, commands and handlers that we support

#include <xc.inc>
#include "constants.h"

#define __DEFINES_ONLY__
#include "mib_definitions.h"
#undef __DEFINES_ONLY__


global _test_endpoint1, _test_endpoint2

;Define the number of supported features and where to store the callback table pointer
;in ROM
#define kNumFeatures 			1
#define kNumDebugCommands		2

PSECT mibmap,abs,local,class=CODE,delta=2

;High memory command structure for processing mib slave endpoints
org 	kMIBEndpointAddress

;Module Name (must be exactly 8 characters long)
db 		0,0,0,0,0,0,0,0

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
goto _test_endpoint1
goto _test_endpoint2

mibfeatures:
BRW
RETLW 	10

mibcommands:
BRW
RETLW 0
RETLW kNumDebugCommands

mibspecs:
BRW
;Debug Feature
RETLW 0
RETLW 0