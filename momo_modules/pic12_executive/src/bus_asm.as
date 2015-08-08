#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "executive.h"

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
;Uses: WREG
;Modifies: CARRY
;Side Effects: CARRY bit is set if the bus is idle and clear otherwise
BEGINFUNCTION _bus_is_idle
	;Check if both SCL and SDA are high
	banksel I2CPORT
	bsf CARRY

	;Check for 40 us to make sure that there are no SCL or SDA transitions
	;or low values in that time.  Since we have a 1mhz clock, if someone is using the
	;bus, we will see either clock go low or data go low (or stay low) once every 100 khz
	;bus period
	movlw 8
	
	loop_check:
	btfss SCLPIN
		bcf CARRY
	btfss SDAPIN
		bcf CARRY
	decfsz WREG,w
		goto loop_check

	return
ENDFUNCTION _bus_is_idle