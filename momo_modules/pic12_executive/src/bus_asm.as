#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "constants.h"
#include "executive.h"
#define __DEFINES_ONLY__
#include "mib_definitions.h"
#undef  __DEFINES_ONLY__

ASM_INCLUDE_GLOBALS()

global _bus_init, _i2c_enable, _bus_is_idle

PSECT text_bus_asm,local,class=CODE,delta=2

;Given our address in WREG, initialize i2c and the mib
BEGINFUNCTION _bus_init
	call _i2c_enable
	
	banksel _status
	bsf BANKMASK(_status), FirstReadBit
	return
ENDFUNCTION _bus_init

;Detect if the i2c bus is idle so that we can start a master MIB packet. The bus
;is defined to be idle if a stop is the last condition on the bus or if neither
;a start nor a stop has been seen and the bus SCL and SDA pins are high
;Uses: None
;Modifies: CARRY
;Side Effects: CARRY bit is set if the bus is idle and clear otherwise
BEGINFUNCTION _bus_is_idle
	bcf CARRY
	banksel SSP1STAT
	btfsc BANKMASK(SSP1STAT), SSP1STAT_S_POSN
		return
	btfsc BANKMASK(SSP1STAT), SSP1STAT_P_POSN
		goto stop_seen

	;Check if both SCL and SDA are high
	banksel I2CPORT
	bsf CARRY
	btfss SCLPIN
		bcf CARRY
	btfss SDAPIN
		bcf CARRY
	return

	stop_seen:
	bsf CARRY
	return
ENDFUNCTION _bus_is_idle