;command_map.asm
;3 structures defining the features, commands and handlers that we support

;#include <xc.inc>
#include "constants.h"
#include "definitions.h"

;Define the number of supported features and where to store the callback table pointer
;in ROM
#define kNumFeatures 			0

;Define ways to create parameter specs simply
#define plist_param_n(n, type) 					((type & 0x01) << (n+3))
#define plist_1param(type) 						plist_param_n(0, type)
#define plist_2params(type1, type2) 			(plist_param_n(0, type1) | plist_param_n(1, type2))
#define plist_3params(type1, type2, type3) 		(plist_param_n(0, type1) | plist_param_n(1, type2) | plist_param_n(2, type3))
#define plist_define(count, params) 			((count & 0b111) | params)

#define plist_define0()							plist_define(0, 0)
#define plist_define1(type)						plist_define(1, plist_1param(type))
#define plist_define2(type1, type2)				plist_define(2, plist_2params(type1, type2))
#define plist_define3(type1, type2, type3)		plist_define(3, plist_3params(type1, type2, type3))

PSECT mibmap,abs,ovrld,local,class=CODE,delta=2

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

mibfeatures:
BRW
RETLW 	0

mibcommands:
BRW
RETLW 0
RETLW 0

mibspecs:
BRW
RETLW 0