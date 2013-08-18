;command_map.asm
;3 structures defining the features, commands and handlers that we support

#include <xc.inc>
#include "constants.h"
#include "definitions.h"

global _flowmeter_statuscmd, _flowmeter_readcmd, _flowmeter_oneshotcmd

;Define the number of supported features and where to store the callback table pointer
;in ROM
#define kNumFeatures 			1
#define kNumCommands			3

PSECT mibmap,abs,local,class=CODE,delta=2

;High memory command structure for processing mib slave endpoints
org 	kMIBEndpointAddress

;Module Name (must be exactly 8 characters long)
db 		ModuleName

;Module information
retlw 	ModuleType
retlw 	ModuleVersion
retlw 	ModuleFlags

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
goto _flowmeter_statuscmd
goto _flowmeter_readcmd
goto _flowmeter_oneshotcmd

mibfeatures:
BRW
RETLW 	12

mibcommands:
BRW
RETLW 0
RETLW kNumCommands

mibspecs:
BRW
retlw plist_no_buffer(1)
retlw 0
retlw plist_no_buffer(1)