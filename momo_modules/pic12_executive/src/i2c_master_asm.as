;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead.  These are routines that
;are only used for the master i2c mode.


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"
#include "protocol_defines.h"

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
;Arguments: None
;Uses: FSR0L, FSR0H, W
;Modifies: bankX, DC, C, Z
;Side Effects: DC set if a collision occurred otherwise clear
BEGINFUNCTION _i2c_master_send_message
	;we want to write to the address
	banksel _mib_state
	movf BANKMASK(send_address),w
	bcf CARRY 							
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
	xorlw _mib_packet + kMIBMessageSize
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

;Receive either 2 or 25 bytes of a MIB response packet.
;Sends a repeated start, address + read and then proceeds to read WREG
;bytes, NACKing after the last byte.  Note that no collisions are possible during a
;read because, if two senders were both transmitting, the collision would have occured
;during the sender address transmission part of the transmission.
;Arguments: WREG, address of the final byte to read (so if you want to read 2 bytes,
;			W=_mib_pacjet + 2 - 1.
;Uses: FSR0, FSR1L
;Modifies BANKX
BEGINFUNCTION _i2c_master_receive_message
	;Save off the number of bytes to read
	movwf FSR1L

	;Send the address with read indication
	banksel _mib_state
	movf BANKMASK(send_address),w
	bsf CARRY
	call _i2c_master_start_address

	call _i2c_loadbuffer

	readloop:

	;Receive one byte, nacking if it is the last byte to read
	;FSR0 always points to bank 0 so we can ignore FSR0H
	bsf CARRY
	movf FSR1L,w
	xorwf FSR0L,w
	btfsc ZERO
		bcf CARRY
	call _i2c_master_receivebyte
	movwi FSR0++

	;Check if we're done, otherwise read another byte
	;i2c_master_receivebyte does not change CARRY, so it retains the
	;same value as we set above.
	btfsc CARRY
		goto readloop

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