;Assembly routines for porting main.c over from xc8 c to assembly code.
;We need to do this because xc8 is terrible and uses too much memory for
;stupid things like moving W to a GPR register and back just to make a
;zero flag appear.   


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "mib12_block.h"

ASM_INCLUDE_GLOBALS()

global _get_magic, _wdt_delay, _bus_init, _register_module

#define k1SecondTimeout 0b010100

PSECT text_main_asm,local,class=CODE,delta=2

;Initialize MIBExecutiveStatus and take action based
;on the state that the chip is in.  Enter the bootloader
;if that's indicated.

BEGINFUNCTION _restore_status
	;Initialize the bits that we know the value for
	banksel _status
	bcf BANKMASK(_status), ValidAppBit
	bsf BANKMASK(_status), SlaveActiveBit

	;Check if we have a valid application module and set status
	call _get_magic
	xorlw kMIBMagicNumber
	btfsc ZERO
		bsf BANKMASK(_status), ValidAppBit

	;check if we should bootload
	call _get_magic
	xorlw kReflashMagicNumber
	btfsc ZERO
		bsf BANKMASK(_status), BootloadBit

	;If we've already registered, we're done, otherwise register
	btfsc BANKMASK(_status), RegisteredBit
		return

	;Go through the registration process
	
	;sleep for 1 second
	movlw k1SecondTimeout
	call _wdt_delay

	movlw kMIBUnenumeratedAddress
	call _bus_init

	call _register_module
	iorlw 0 ;make sure the zero flag is set
	
	;If address was zero, there was an error, return
	btfsc ZERO
		return

	;Reinitialize with the address (in W) and record that we're registered
	banksel _status
	bsf BANKMASK(_status), RegisteredBit
	goto _bus_init
ENDFUNCTION _restore_status

;Kernel panic function
;Move WREG to EEDATL for safekeeping and loop forever
;Set trap bit so that RPC callers can tell we're dead
BEGINFUNCTION _trap
	banksel EEDATL
	movwf 	BANKMASK(EEDATL)
	banksel _status
	bsf 	BANKMASK(_status),TrapBit
	loop_forever:
	sleep
	goto loop_forever
ENDFUNCTION _trap