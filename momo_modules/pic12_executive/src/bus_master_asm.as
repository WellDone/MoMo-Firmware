;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"

ASM_INCLUDE_GLOBALS()

global _i2c_master_receive_message, _i2c_master_send_message, _i2c_loadbuffer
global _bus_is_idle, _i2c_finish_transmission, _i2c_set_master_mode
global _i2c_append_checksum
PSECT text_bus_master,local,class=CODE,delta=2

;Capture the I2C bus once it has become idle and prepare it to send a master MIB
;call.  
;Arguments: None
;Modifies: C,Z
;Returns: Nothing
;Side Effects: None
BEGINFUNCTION _bus_master_begin_rpc
	banksel _mib_state
	movwf BANKMASK(send_address)

	;Wait until the bus is idle
	call _bus_is_idle
	btfss CARRY
		goto $-2

	movlw 1
	goto _i2c_set_master_mode
ENDFUNCTION _bus_master_begin_rpc

BEGINFUNCTION _bus_master_tryrpc
	banksel _mib_state
	movf BANKMASK(send_address),w
	call _i2c_master_send_message
	
	;Keep attempting to read until we either are successful or have to stop
	;because of a collision or invalid sent packet 
	attempt_read:
	banksel _mib_state
	movf BANKMASK(send_address),w
	call _i2c_master_receive_message
	btfsc DC
		return
	btfsc CARRY
		goto attempt_read

	return
ENDFUNCTION _bus_master_tryrpc

;Send a master RPC to the address specified in W.  Keep trying until successful.
;Uses: FSR0, FSR1, W
;Modifies: DC,C,Z
;Side Effects: 
;Returns: Status code from the call in W
BEGINFUNCTION _bus_master_send_rpc
	;Load from address into the appropriate spots and append a checksum
	banksel _mib_state
	movf BANKMASK(slave_address),w
	movwf BANKMASK(bus_sender)
	call _i2c_append_checksum

	wait_and_start:
	banksel PIR1
	bcf BCL1IF
	
	;Wait for the bus to be idle (CARRY set when bus is idle)
	call _bus_is_idle
	btfss CARRY
		goto wait_and_start

	call _bus_master_tryrpc
	btfsc DC
		goto wait_and_start

	call _i2c_finish_transmission

	movlw 0
	call _i2c_set_master_mode
	
	banksel bus_status	
	movf 	BANKMASK(bus_status), w
  	return 
ENDFUNCTION _bus_master_send_rpc