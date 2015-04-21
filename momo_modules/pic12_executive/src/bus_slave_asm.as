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

PSECT text_bus_slave,local,class=CODE,delta=2

;Given the returnvalue in WREG set the bus slave return status to that value
;and update the bus status checksum to the correct checksum value.
;Uses:WREG, FSR0L
;Returns:Nothing
;Modifies: Z,bank1
BEGINFUNCTION _bus_slave_setreturn
	banksel bus_status
	movwf 	BANKMASK(bus_status)
	comf	WREG,w
	incf	WREG,w
	movwf 	BANKMASK(bus_statuscheck)
	return
ENDFUNCTION _bus_slave_setreturn

BEGINFUNCTION _bus_slave_callcommand
	call _i2c_verify_checksum
	skipz
		goto checksum_error

	call _bus_slave_callhandler
	goto _bus_slave_setreturn
	;FIXME: Have the return value of the slave handler become the bus_slave_setreturn argument
	;Function returns normally

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
	;load in the application's command table
	btfss _status, ValidAppBit
		retlw kCommandNotFoundStatus

	;Load in the application command table
	call mib_block_address(kMIBCommandMapGotoOffset)

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

	;We found a match, call this command handler
	moviw [3]FSR0
	movwf PCLATH
	moviw [2]FSR0
	callw
	bsf WREG, kAppDefinedBit
	return

	next_cmd:
	addfsr FSR0, 4
	goto look_for_command
ENDFUNCTION _bus_slave_callhandler
