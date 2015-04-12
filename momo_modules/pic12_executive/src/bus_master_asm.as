;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"

ASM_INCLUDE_GLOBALS()

global _i2c_master_receive_message, _i2c_master_send_message, _i2c_loadbuffer
global _bus_is_idle, _i2c_finish_transmission, _i2c_set_master_mode
global _i2c_append_checksum, _wdt_delay, _i2c_verify_checksum

PSECT text_bus_master,local,class=CODE,delta=2

;Capture the I2C bus once it has become idle and prepare it to send a master MIB
;call.  
;Arguments: Address to send to in WREG
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

;Attempt to send an RPC packet and return with DC set if there was a collision.
;Transient checksum errors are automatically retried until they succeed.
BEGINFUNCTION _bus_master_tryrpc
	;Send the command, returning with DC set if there was a collision
	call _i2c_master_send_message
	btfsc DC
		return
	
	;Save off the first byte of the command packet since that will be overwritten
	;when we read the return status and if the status says that the command checksum
	;was bad, we need to resend the complete command including the overwritten byte.
	banksel _mib_state
	movf 	BANKMASK(_mib_packet + 0),w
	movwf 	FSR1H

	;Keep attempting to read until we either are successful or have to stop
	;because of a collision or invalid sent packet 
	read_status:

	;Read 2 bytes to get the command status
	movlw _mib_packet + 2 - 1
	call _i2c_master_receive_message

	;Check if the first two bytes were 0xFF, 0xFF indicating the slave was nonexistant.
	moviw [-2]FSR0
	xorlw 0xFF
	btfss ZERO
		goto verify_checksum
	moviw [-1]FSR0
	xorlw 0xFF
	btfsc ZERO
		return

	;Check if the checksum is valid, or if we need to reread the two byte status code
	verify_checksum:
	addfsr FSR0, -2
	moviw FSR0++
	addwf INDF0,w
	btfss ZERO
		goto read_status
	
	;Check if the slave told us that we need to resend the command
	moviw [-1]FSR0 ;get the return status
	xorlw kChecksumMismatchStatus
	btfsc ZERO
		goto resend_command_error

	;Check if there is any data to receive, if not we're done.
	moviw [-1]FSR0
	btfss WREG, 6
		return

	;Read the whole 25 byte packet
	read_packet:
	movlw _mib_packet + kMIBMessageSize - 1
	call _i2c_master_receive_message

	;Verify the checksum of the entire packet
	call _i2c_loadbuffer
	call _i2c_verify_checksum
	btfsc ZERO
		goto read_packet

	;Otherwise we're done, having successfully read the 25 byte packet from the wire
	return

	;If we need to resend the command, restore the first byte of the command packet that we 
	;temporarily overwrote and return with DC set to indicate that we should resend.
	;Send a stop since we'll try to grab the bus again and send a start when we retransmit the
	;command packet.
	resend_command_error:
	movf FSR1H,w
	movwi [-2]FSR0
	call _i2c_finish_transmission
	bsf DC
	return
ENDFUNCTION _bus_master_tryrpc

;Send a master RPC to the address specified by slave_address.  Keep trying until successful.
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

	bcf BCL1IF
	
	;Wait for the bus to be idle (CARRY set when bus is idle)
	call _bus_is_idle
	btfss CARRY
		goto _bus_master_send_rpc

	call _bus_master_tryrpc
	btfsc DC
		goto _bus_master_send_rpc

	call _i2c_finish_transmission

	movlw 0
	call _i2c_set_master_mode
	
	banksel bus_status	
	movf 	BANKMASK(bus_status), w
  	return 
ENDFUNCTION _bus_master_send_rpc
