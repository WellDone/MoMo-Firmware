;command_map.asm
;3 structures defining the features, commands and handlers that we support

#include <xc.inc>
#include "constants.h"
#include "definitions.h"

global _gsm_module_on, _gsm_sendcommand, _gsm_power_on, _gsm_dumpbuffer
global _gsm_openstream,_gsm_putstream,_gsm_closestream, _gsm_testsim

;Define the number of supported features and where to store the callback table pointer
;in ROM
#define kNumFeatures 			2
#define kNumDebugCommands		5
#define kNumStreamCommands		3

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
goto _gsm_module_on
goto _gsm_power_on
goto _gsm_sendcommand
goto _gsm_testsim
goto _gsm_dumpbuffer

goto _gsm_openstream
goto _gsm_putstream
goto _gsm_closestream

mibfeatures:
BRW
RETLW 	10
RETLW 	11

mibcommands:
BRW
RETLW 0
RETLW kNumDebugCommands
RETLW kNumStreamCommands+kNumDebugCommands

mibspecs:
BRW
;Debug Feature
RETLW 0
RETLW 0
RETLW plist_buffer()
RETLW 0
RETLW 0
RETLW 0

;Stream Feature
RETLW plist_buffer()
RETLW plist_buffer()
RETLW 0