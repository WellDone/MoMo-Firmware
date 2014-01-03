;command_map.asm
;3 structures defining the features, commands and handlers that we support

;#include <xc.inc>
#include "constants.h"
#include "definitions.h"

jumpm MACRO name,dest
name:
	pagesel(dest)
	goto  dest && ((1<<11) - 1)
	
ENDM

global _sample_v1, _sample_i1,_log_getoffset,_log_getsector,_open_size

;Define the number of supported features and where to store the callback table pointer
;in ROM
#define kNumFeatures 			1

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

PSECT mibstructs,local,class=CONST,delta=2
jumptable:
jumpm checksize,_open_size
jumpm samplei,_sample_i1
jumpm checkoff,_log_getoffset
jumpm checksect,_log_getsector

mibhandlers:
BRW
goto checksize
goto samplei
goto checkoff
goto checksect

mibfeatures:
BRW
RETLW 	20

mibcommands:
BRW
RETLW 0
RETLW 4

mibspecs:
BRW
RETLW 0
RETLW 1<<5
RETLW 0
RETLW 0

PSECT mibmap,abs,ovrld,local,class=CODE,delta=2

;High memory command structure for processing mib slave endpoints

org 	kMIBEndpointAddress - 8
jumpm mf,mibfeatures
jumpm mc,mibcommands
jumpm ms,mibspecs
jumpm mh,mibhandlers

;Module Name (must be exactly 8 characters long)
db 		ModuleName

;Module information
retlw 	ModuleType
retlw 	ModuleVersion
retlw 	ModuleFlags

;MIB endpoint information
retlw 	kNumFeatures
goto 	mf
goto 	mc
goto 	ms
goto 	mh
retlw	kMIBMagicNumber