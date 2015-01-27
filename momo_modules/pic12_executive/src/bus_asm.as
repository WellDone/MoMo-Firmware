#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "constants.h"
#include "executive.h"
#define __DEFINES_ONLY__
#include "mib_definitions.h"
#undef  __DEFINES_ONLY__

ASM_INCLUDE_GLOBALS()

global _bus_init, _i2c_enable

#define kInvalidMIBIndex 255

PSECT text_bus_asm,local,class=CODE,delta=2

;Given our address in WREG, initialize i2c and the
;mib
BEGINFUNCTION _bus_init
	call _i2c_enable
	
	banksel _mib_state
	bsf BANKMASK(_status), FirstReadBit
	movlw kInvalidMIBIndex
	movwf BANKMASK(slave_handler)
	return
ENDFUNCTION _bus_init