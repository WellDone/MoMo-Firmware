;command_map.asm
;3 structures defining the features, commands and handlers that we support

\#include "constants.h"

;All MIB endpoints are defined in other files so they must be declared global here.
#for $feat in $features.keys()
	#for $cmd in $features[$feat]
global $cmd.symbol
	#end for
#end for

;Define the number of supported features and where to store the callback table pointer
;in ROM

;High memory command structure for processing mib slave endpoints

PSECT mibblock,local,class=CODE,delta=2
;Module information
retlw 	kModuleHWType			;The HW type that this application module runs on
retlw 	$module_type			;ModuleType
retlw 	1<<4 | ($flags & 0xFF) 	;Nibble for MIB Revision, Nibble for Module Flags

;Module Name (must be exactly 7 characters long)
#for $i in $range(0, $len($name))
retlw 	$ord($name[$i])
#end for

;MIB endpoint information
retlw 	$num_features
goto 	mibfeatures
goto 	mibcommands
goto 	mibspecs
goto 	mibhandlers
retlw	kMIBMagicNumber

PSECT mibstructs,local,class=CONST,delta=2

mibhandlers:
brw
#for $feat in $features.keys()
	#set $cmd_cnt = 0
	#for $cmd in $features[$feat]
	goto $cmd.symbol			; Feature $feat, Command $cmd_cnt
	#set $cmd_cnt = $cmd_cnt + 1
	#end for
#end for


mibfeatures:
brw
#for $feat in $features.keys()
	retlw $feat
#end for

#set $cmd_cnt = 0
mibcommands:
brw
#for $feat in $features.keys()
	retlw $cmd_cnt
	#set $cmd_cnt = $cmd_cnt + $len($features[$feat])
#end for
	retlw $cmd_cnt

mibspecs:
brw
#for $feat in $features.keys()
	#for $cmd in $features[$feat]
	retlw $cmd.spec
	#end for
#end for