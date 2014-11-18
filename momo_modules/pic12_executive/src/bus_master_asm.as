;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"

ASM_INCLUDE_GLOBALS()

global _i2c_master_receive_message, _i2c_master_send_message, _i2c_loadbuffer
global _bus_is_idle, _i2c_finish_transmission, _i2c_set_master_mode
PSECT text_bus_master,local,class=CODE,delta=2

BEGINFUNCTION _bus_master_tryrpc
	;FIXME load in address to W	
	call _i2c_master_send_message
	
	;Keep attempting to read until we either are successful or have to stop
	;because of a collision or invalid sent packet 
	attempt_read:
	call _i2c_master_receive_message
	btfsc DC
		return
	btfsc CARRY
		goto attempt_read

	return
ENDFUNCTION _bus_master_tryrpc

;Send a master RPC to the address specified in W.  Keep trying until successful
;Uses: FSR0, FSR1, W
;Modifies: DC,C,Z
;Side Effects: 
;Returns: Status code from the call in W
BEGINFUNCTION _bus_master_send_rpc
	;FIXME insert addreses and checksum here
	wait_and_start:
	banksel PIR1
	bcf BCL1IF
	
	;Wait for the bus to not be idle
	call _bus_is_idle
	xorlw 0
	btfsc ZERO
		goto wait_and_start

	call _bus_master_tryrpc
	btfsc DC
		goto wait_and_start

	call _i2c_finish_transmission
	movlw 0
	call _i2c_set_master_mode
	goto _get_mib_result
ENDFUNCTION _bus_master_send_rpc

BEGINFUNCTION _get_mib_result
	banksel bus_status	
	movf 	BANKMASK(bus_status), w
  	return 
ENDFUNCTION _get_mib_result