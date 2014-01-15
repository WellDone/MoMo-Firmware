;registration_asm.as

#include <xc.inc>
#include "asm_macros.inc"
#include "constants.h"
#include "asm_branches.inc"

#define __DEFINES_ONLY__
#include "mib_definitions.h"

global _mib_to_fsr0, _get_magic, _get_mib_block
global _copy_fsr1_to_fsr0
global _mib_data

#define _mib_buffer (_mib_data+3)

PSECT text_asm_register,local,class=CODE,delta=2

/*BEGINFUNCTION _register_module
	;check if there's a valid application module loaded
	call _get_magic
	xorlw kMIBMagicNumber
	skipnz
		goto app_register
	
	;no app module, send default data
	movlw module_info & 0xFF
	movwf FSR1L
	movlw 1 << 7 | ((module_info >> 8) & 0xFF)
	movwf FSR1H
	goto copy_and_send

	app_register:
	;movlw 0 not needed because w is 0 from xor above
	call _get_mib_block

	copy_and_send:
	call _mib_to_fsr0
	movlw kMIBPic12lf1822
	movwi FSR0++
	movlw kModuleDescriptorSize-1	;we copied the hw type above
	call _copy_fsr1_to_fsr0

	;MIB now has module descriptor packet
	;TODO bus master prepare and send 

ENDFUNCTION _register_module

BEGINREGION module_info
retlw kMIBExecutiveOnlyType
retlw kMIBVersion1
retlw 0
db	  'N','O',' ','A','P','P',' ',' '
retlw 1
ENDREGION module_info
*/