;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"
#include "asm_branches.inc"
#include "protocol_defines.h"
#include "mib12_block.h"

ASM_INCLUDE_GLOBALS()

global _i2c_verify_checksum
global exec_new_cmd_map
global _bus_slave_returndata

PSECT text_bus_slave,local,class=CODE,delta=2

;Given the returnvalue in WREG set the bus slave return status to that value
;and update the bus status checksum to the correct checksum value.
;Uses:WREG, FSR0L
;Returns:Nothing
;Modifies: Z,bank1
BEGINFUNCTION _bus_slave_setreturn
	banksel bus_status
	movwf 	BANKMASK(bus_status)
	goto 	_bus_slave_update_statuscheck
ENDFUNCTION _bus_slave_setreturn

BEGINFUNCTION _bus_slave_update_statuscheck
	banksel bus_status
	movf 	BANKMASK(bus_status),w
	comf	WREG,w
	incf	WREG,w
	movwf 	BANKMASK(bus_statuscheck)
	return
ENDFUNCTION _bus_slave_update_statuscheck

;Set the length of data returned by the slave endpoint
;Arguments: length of data to return in WREG
;Returns: Nothing
BEGINFUNCTION _bus_slave_returndata
	banksel bus_status
	movwf BANKMASK(bus_length)
	bsf   BANKMASK(bus_status), kHasDataBit
	return
ENDFUNCTION _bus_slave_returndata

BEGINFUNCTION _bus_slave_callcommand
	call _i2c_verify_checksum
	skipz
		goto checksum_error

	;Save off the sender's address so we know who to respond to in case
	;this is an asynchronous call.
	banksel(bus_sender)
	movf BANKMASK(bus_sender),w
	movwf BANKMASK(send_address)

	;Make sure we initialize the bus_status
	;which is shared with the param length, only clear the high order bits
	;that cannot be shared with the length since the length is limited to 20 bytes
	bcf BANKMASK(bus_status), kHasDataBit
	bcf BANKMASK(bus_status), kAppDefinedBit

	;Try to call the handler.  Whatever return status was given,
	;combine that with what is in bus_status in case the slave handler
	;called bus_slave_returndata, which will set the kHasData bit
	call _bus_slave_callhandler
	
	banksel bus_status
	btfsc BANKMASK(bus_status), kHasDataBit
		bsf WREG, kHasDataBit
	movwf bus_status

	;Check if the handler told us to respond asynchronously
	xorlw kAsynchronousResponseCode | (1 << 6)
	btfsc ZERO
		goto async_response

	goto _bus_slave_update_statuscheck

	;Asynchronous Response
	;Indicate that we're in the middle of an asynchronous response.  We can't set the
	;busy bit here directly because otherwise we won't be able to process interrupts related
	;to clocking out this response.
	async_response:
	bsf BANKMASK(_status), AsyncBit

	;Call setreturn again to make sure that we don't have the HasData bit set
	movlw kAsynchronousResponseStatus
	goto _bus_slave_setreturn

	;Errors
	checksum_error:
	movlw kChecksumMismatchStatus
	goto _bus_slave_setreturn
ENDFUNCTION _bus_slave_callcommand

;Given the packet that we currently have in the mib_buffer,
;attempt to locate and call the corresponding handler.  If the
;handler can be found, call it. Otherwise set an appropriate error code.
;
;Uses: WREG, FSR0, FSR1
;Return: Error code or command return value in WREG
BEGINFUNCTION _bus_slave_callhandler
	banksel bus_cmdlo

	;Load executive command map location
	movlw low exec_new_cmd_map
	movwf FSR0L
	movlw (high exec_new_cmd_map) | (1 << 7)
	movwf FSR0H

	;Check if this command corresponds to an executive command
	;and skip over loading the application cmd table if so
	movlw  kExecutiveSpecialHighByte
	xorwf  bus_cmdhi,w
	btfsc  ZERO
		goto look_for_command

	;Check if we have a valid application loaded and if so
	;load in the application's command table otherwise there's no way
	;the command could exist so just fail now.
	btfss _status, ValidAppBit
		retlw kCommandNotFoundStatus

	;Load in the application command table
	movlw kMIBCommandMapIndex
	call mib_block_address(kMIBApplicationInfoOffset)

	;Do a linear search through the commands that we have in the active
	;command table that is loaded into FSR0 and see if any match our
	;desired command id.  If so, call the command and return what it
	;returns, indicating that the result came from a user controller function.
	;Otherwise return command not found if we get through the entire table.
	look_for_command:

	;If the high byte of the handler address is a sentinel value that can never be valid,
	;we're at the end of the list and so we were not successful in finding the command handler.
	moviw [3]FSR0
	xorlw kMIBCommandMapSentinelValue
	btfsc ZERO
		retlw kCommandNotFoundStatus

	;Check if the 2 byte command id matches exactly with what we were sent.
	moviw [0]FSR0
	xorwf bus_cmdlo,w
	btfss ZERO
		goto next_cmd
	moviw [1]FSR0
	xorwf bus_cmdhi,w
	btfss ZERO
		goto next_cmd

	;We found a match, call this command handler and return whatever it returns.
	;Mask out the high bits since those have special meanings that the application
	;cannot directly affect.
	moviw [3]FSR0
	movwf PCLATH
	moviw [2]FSR0
	callw
	movlp 0		;PCLATH must be cleared so that the next goto doesn't shoot off into randomness

	;Make sure the application endpoint does not accidently overwrite any of our special bits
	andlw kMIBStatusCodeMask
	bsf WREG, kAppDefinedBit
	return

	next_cmd:
	addfsr FSR0, 4
	goto look_for_command
ENDFUNCTION _bus_slave_callhandler
