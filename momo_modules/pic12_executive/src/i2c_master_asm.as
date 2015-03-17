;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead.  These are routines that
;are only used for the master i2c mode.


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"
#include "bus_defines.h"

ASM_INCLUDE_GLOBALS()

global _i2c_loadbuffer
global _i2c_calculate_checksum, _i2c_verify_checksum

PSECT i2c_master,local,class=CODE,delta=2

;Wait synchronously for a master i2c operation to complete by polling SSP1IF
;If a bus collision occurs, return with Digit Carry flag set, otherwise return 
;with DC flag clear
;Uses: None
;Modifies: DC, bank0
;Side Effects: 	DC flag set if bus collision occurred, DC flag clear if bus collision
;				did not occur
BEGINFUNCTION _i2c_wait_flag
	banksel PIR1
	bcf DC

	check_flag_loop:
	btfsc SSP1IF
		goto flag_done
	btfsc BCL1IF
		goto collision
	goto check_flag_loop

	collision:
	bsf DC

	flag_done:
	goto _clear_ssp1if
ENDFUNCTION _i2c_wait_flag

;Clear the SSP1IF interrupt flag
;Uses: None
;Modifies: bank0
;Returns: SSP1IF flag is clear 
BEGINFUNCTION _clear_ssp1if
	banksel PIR1
	bcf SSP1IF
	return
ENDFUNCTION _clear_ssp1if

;Given an address to talk to in W and a direction indicated by DC
;send a start or repeated start and synchronously wait to send the
;address.
;Uses: None
;Arguments: Address to send to in W (not shifted), C set if read
;C clear if write
;Modifies: DC, W, bankX
;Side Effects: DC set if a collision occurred, DC clear otherwise
BEGINFUNCTION _i2c_master_start_address
	call _clear_ssp1if 

	;Send start for write, repeated start for read
	banksel SSP1CON2
	btfss CARRY
		bsf BANKMASK(SSP1CON2), 0 ;Start bit
	btfsc CARRY
		bsf BANKMASK(SSP1CON2), 1 ;Repeated start bit

	;Wait for start to be sent, if a collision occured return
	call _i2c_wait_flag
	btfsc DC
		return

	;Prepare the address in W by shifting it and including R/W bit
	;Since CARRY is 1 for read and 0 for right a shift with carry is enough
	rlf WREG,w 
	banksel SSP1CON2
	movwf BANKMASK(SSP1BUF)
	goto _i2c_wait_flag
ENDFUNCTION _i2c_master_start_address

;Given an address to talk to in W, send the command and parameters
;buffer to that address.  Return once the packet is sent or if there
;is an error in transmission.
;Arguments: Address to send to in W (not shifted)
;Uses: FSR0L, FSR0H, W
;Modifies: bankX, DC, C, Z
;Side Effects: DC set if a collision occurred otherwise clear
BEGINFUNCTION _i2c_master_send_message
	bcf CARRY 							;we want to write to the address
	call _i2c_master_start_address
	btfsc DC
		return

	;Use FSR0 as the indirect register holding the data we want to send
	;We always send a complete buffer (4 byte header + 20 byte packet + 1 byte checksum)
	;so transmission is done once we have sent 25 bytes
	call _i2c_loadbuffer

	sendloop:
	banksel SSP1BUF
	moviw FSR0++
	movwf BANKMASK(SSP1BUF)
	call _i2c_wait_flag
	btfsc DC
		return
	movf FSR0L,w
	xorlw _mib_data + kBusPacketSize + 1
	btfss ZERO
		goto sendloop

	return
ENDFUNCTION _i2c_master_send_message

;Receive one byte from i2c as a master and return it in W
;If CARRY is set, acknowledge the byte, otherwise send a NACK 
BEGINFUNCTION _i2c_master_receivebyte
	;start receiving the byte
	banksel SSP1CON2
	bsf BANKMASK(SSP1CON2), 3 ;start receiving a byte
	call _i2c_wait_flag
	btfsc DC
		return

	;Copy the byte to W
	banksel SSP1BUF
	movf BANKMASK(SSP1BUF),w

	;If carry is set, ACK the byte, otherwise NACK
	;SSP1CON2,5 is active low
	bsf BANKMASK(SSP1CON2), 5
	btfsc CARRY
		bcf BANKMASK(SSP1CON2), 5

	bsf BANKMASK(SSP1CON2), 4 		;send the ACK/NACK
	goto _i2c_wait_flag
ENDFUNCTION _i2c_master_receivebyte

;Receive a MIB response packet.  Read the first 2 bytes and check for a valid
;checksum before reading the body of the packet to make sure that we don't overwrite
;the command that we sent with garbage data in case the slave is telling us that
;there was a corruption in transmission or we couldn't receive the slave's response correctly.
;Arguments: Address to send to in W (not shifted)
;Uses: FSR0, FSR1, W
;Modifies: bankX, DC, Z
;Side Effects: 	If the command needs to be resent, DC is set.  
; 				If the response needs to be reread, C is set. 			
BEGINFUNCTION _i2c_master_receive_message
	;Start a receive packet (address in W + read indication in CARRY bit)
	bsf CARRY
	call _i2c_master_start_address
	btfsc DC
		return

	;Setup FSR0 with _mibdata
	call _i2c_loadbuffer	

	;save off first two bytes of packet since those will be overwritten
	;and may need to be restored if the slave response is that our command
	;was corrupted in transmission.
	moviw [0]FSR0
	movwf FSR1L
	moviw [1]FSR0
	movwf FSR1H

	;Attempt to read 25 bytes
	readloop:

	;Acknowledge bytes 0-23 and do no acknowledge last byte 
	bsf CARRY
	movlw _mib_data + kBusPacketSize
	xorwf FSR0L,w
	btfsc ZERO
		bcf CARRY
	call _i2c_master_receivebyte
	
	movwi FSR0++
	btfsc DC
		goto resend_command_error

	;After receiving 2 bytes, check to make sure we should keep reading
	movlw _mib_data + 2
	xorwf FSR0L,w
	btfsc ZERO
		goto check_status

	;If we have read 25 bytes, we're done
	movlw _mib_data + kBusPacketSize + 1
	xorwf FSR0L,w
	btfsc ZERO
		goto done_reading

	goto readloop

	;Check to make sure the first two bytes are twos complements of each other
	;and that the return status is not checksum error
	;FSR0[-2] has the return status
	;FSR0[-1] has the checksum
	check_status:
	;Check if the return status was 0xFF indicating the slave was nonexistant.
	moviw [-2]FSR0
	xorlw 0xFF
	btfss ZERO
		goto verify_status_checksum

	;The slave is not there, the bus is just giving us 0xFF, 0xFF
	;Initiate one more read with a NACK to follow i2c spec
	bcf CARRY
	call _i2c_master_receivebyte
	goto finished_call

	verify_status_checksum:
	addfsr FSR0, -2
	moviw FSR0++
	addwf INDF0,w
	;If the checksum is valid, W should be 0
	btfss ZERO
		goto checksum_error

	;If slave responded that our command was corrupted, try again
	moviw [-1]FSR0 ;get the return status
	xorlw kChecksumError
	btfsc ZERO
		goto resend_command_error

	;If the 2nd MSB of the slave's response is 0, then there is no more
	;data to read because either there was an error or because it simply
	;didn't return data beyond a status code.
	moviw [-1]FSR0
	btfss WREG, 6
		goto no_data_to_read

	;move the pointer back to the next byte location and continue the loop
	addfsr FSR0, 1
	goto readloop

	;If the response said that there was an error or no data, then
	;clear the bus and don't read any further.
	no_data_to_read:
	bcf CARRY
	call _i2c_master_receivebyte
	goto finished_call

	;We've read 25 bytes without error, make sure the checksum is valid
	done_reading:
	call _i2c_loadbuffer
	call _i2c_verify_checksum
	btfss ZERO
		goto checksum_error

	;We were successful, so clear the status bits and return	
	finished_call:
	bcf DC
	bcf CARRY
	return

	;The slave has indicated a checksum error receiving the command packet
	;We need to send one more read with a nack to clear the bus and then
	;resend the command.  Set DC to indicate 
	resend_command_error:
	bsf DC

	;We could not read this packet, send one more nack to clear the bus
	;and return with C set so that we know to either retry the read
	;or retry the write (if we can from resend_command_error above)
	checksum_error:
	bcf CARRY
	call _i2c_master_receivebyte
	bsf CARRY

	;Restore the packet portion that was overwritten
	call _i2c_loadbuffer
	movf FSR1L,w
	movwi [0]FSR0
	movwf FSR1H
	moviw [1]FSR0
	return
ENDFUNCTION _i2c_master_receive_message

;Synchronously send a stop condition on the bus.
;Uses: None
;Modifies: DC if a collision occurred
BEGINFUNCTION _i2c_finish_transmission
	call _clear_ssp1if
	banksel SSP1CON2
	bsf BANKMASK(SSP1CON2), 2
	goto _i2c_wait_flag
ENDFUNCTION _i2c_finish_transmission