;i2c_utilities.as
;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_macros.inc"

GLOBAL _mib_state, _i2c_status, _i2c_core_transfer
GLOBAL _i2c_master_receivedata, _i2c_master_receivechecksum
GLOBAL _i2c_slave_address
GLOBAL _i2c_enable

#define bus_address 	_mib_state+3
#define bus_checksum 	_mib_state+4
#define bus_dataptr 	_mib_state+5
#define bus_lastdata 	_mib_state+6
#define i2cerror 		_i2c_status+1

PSECT text_i2c_utils,local,class=CODE,delta=2

#define clearbit(reg, id)	bcf reg&0x7F, reg##_##id##_POSN
#define setbit(reg, id)		bsf reg&0x7F, reg##_##id##_POSN
#define bittfsc(reg, id)	btfsc reg&0x7F, reg##_##id##_POSN

;Set the i2c_status state to either kI2CSendDataState or kI2CReceiveDataState based on whether
;bit 1 of address is set
;i2c_status.state = (((i2c_msg->address) & 0x01) == 0) ? kI2CSendDataState : kI2CReceiveDataState;
;mib_state and i2c_status must be in bank1 and must not change size
BEGINFUNCTION _i2c_choose_direction
	movlb 1
	btfss BANKMASK(_mib_state+3),0 		;test if bit 1 of address is set
	movlw kI2CSendDataState
	btfsc BANKMASK(_mib_state+3),0
	movlw kI2CReceiveDataState
	call _i2c_setstate
	return
ENDFUNCTION _i2c_choose_direction

;move the contents of the wregister to the i2c_status state register
BEGINFUNCTION _i2c_setstate
	movlb 1
	xorwf BANKMASK(_i2c_status),w ;swap state and wreg
	xorwf BANKMASK(_i2c_status),f
	xorwf BANKMASK(_i2c_status),w
	bcf BANKMASK(_i2c_status),7	  ;reset slave state if appropriate
	btfsc WREG,7
	bsf BANKMASK(_i2c_status),7
	return
ENDFUNCTION _i2c_setstate

;get the i2c state and return it in w
BEGINFUNCTION _i2c_getstate
	movlb 1
	movlw 0b01111111
	andwf BANKMASK(_i2c_status),w
	return
ENDFUNCTION _i2c_getstate

;Setup the FSR0 register to point to the dataptr in the i2c message
BEGINFUNCTION i2c_setup_buffer
	movlb 1
	clrf FSR0H
	movf BANKMASK(bus_dataptr),w
	movwf FSR0L
	return
ENDFUNCTION i2c_setup_buffer

BEGINFUNCTION _i2c_receive
	movlb 4
	movf BANKMASK(SSP1BUF),w
	movlb 1
	return
ENDFUNCTION _i2c_receive

BEGINFUNCTION _i2c_transmit
	movlb 4
	movwf BANKMASK(SSP1BUF)
	movlb 1
	return
ENDFUNCTION _i2c_transmit

;Calculate the two's complement of the i2c message checksum 
;and return it in w (storing it in the checksum variable as well)
BEGINFUNCTION _i2c_calc_check
	movlb 1
	comf BANKMASK(bus_checksum),f
	incf BANKMASK(bus_checksum),f
	movf BANKMASK(bus_checksum),w
	return
ENDFUNCTION _i2c_calc_check

;Receive the next byte from i2c wire
;compute checksum and check if this byte matches
;update i2c error status and state to kI2CUserCallbackState

BEGINFUNCTION _i2c_core_receivechecksum
	movlb 1
	clrf BANKMASK(i2cerror)
	call _i2c_calc_check
	call _i2c_receive
	xorwf BANKMASK(bus_checksum),w
	btfsc ZERO
	goto dontseterror
	movlw kI2CInvalidChecksum
	movwf BANKMASK(i2cerror)
	dontseterror:
	movlw kI2CUserCallbackState
	call _i2c_setstate
	return
ENDFUNCTION _i2c_core_receivechecksum

;_i2c_set_mastermode
;enable or disable mastermode according to w
;w = 1 means enable
;w = 0 means disable
BEGINFUNCTION _i2c_set_master_mode
	movwf FSR1H
	movlb 1
	movlw 0
	movwf BANKMASK(_i2c_status)	;set to idle and master
	btfss FSR1H,0
	bsf   BANKMASK(_i2c_status),7 ;set to slave if w==0

	bcf SSP1IE ;bank 1
	movlb 4
	clearbit(SSP1CON2, GCEN)
	clearbit(SSP1CON1, SSPEN)
	clearbit(SSP1CON2, SEN)
	btfss FSR1H,0 		;slave mode SEN=1
	setbit (SSP1CON2, SEN)

	movlw kI2CMasterMode
	btfss FSR1H,0
	movlw kI2CSlaveMode
	movwf BANKMASK(SSP1CON1)

	BANKSEL _i2c_slave_address
	movf 	BANKMASK(_i2c_slave_address),w
	movlb 	4
	btfsc FSR1H,0
	movlw  0x09	 	;in master mode set baud to 100 khz

	movwf BANKMASK(SSP1ADD)

	movlb 0
	bcf SSP1IF
	movlb 1
	bsf SSP1IE
	return
ENDFUNCTION _i2c_set_master_mode

;Enable the i2c interface and configure it for mib use
;put it in slave mode
BEGINFUNCTION _i2c_enable
	lslf WREG,w
	movlb 1
	bsf SDATRIS
	bsf SCLTRIS

	BANKSEL _i2c_slave_address
	movwf BANKMASK(_i2c_slave_address)

	movlb 4
	movlw 0xff & kI2CFlagMask
	movwf BANKMASK(SSP1STAT)
	movlw 0
	call _i2c_set_master_mode
	call _i2c_receive
	return
ENDFUNCTION _i2c_enable


;Send or receive the next byte of data as either master or slave
;Take in a packed uint that defines what state to transition to next
;argument is defined as follows:
;bit 0-2 - dest_state if we're not done with message
;bit 3 - send(1), receive(0)
;bit 4-6 - dest state if we're done with message
;bit 7 - master(1), slave(0)
;Uses: FSR1H to store argument
;Post Condition:
;	next byte in the message is sent or received
;	data pointers are updated to point to next byte
;	state is updated 
;	if we're slave, clock is released
BEGINFUNCTION _i2c_core_transfer
	movwf FSR1H
	call i2c_setup_buffer
	movlb 4
	btfss FSR1H,3		;1 if send
	goto receive
	;send byte
	movf INDF0,w
	movwf BANKMASK(SSP1BUF)
	goto done
	receive:
	movf BANKMASK(SSP1BUF),w
	movwf INDF0
	done:	;in both cases w now contains the last byte sent or received
	btfss FSR1H,7	;if we're in slave mode, release the clock
	setbit(SSP1CON1, CKP)		
	movlb 1
	addwf BANKMASK(bus_checksum),f
	incf BANKMASK(bus_dataptr),f
	movf BANKMASK(bus_dataptr),w
	xorwf BANKMASK(bus_lastdata),w
	btfsc ZERO							;check if we're done with the message
	swapf FSR1H,f						;if we should jump to high nibble, swap it in
	movf FSR1H,w
	andlw 0b111
	call _i2c_setstate
	return
ENDFUNCTION _i2c_core_transfer

BEGINFUNCTION _i2c_master_interrupt
	;if bus collision
	movlb 0
	btfss BCL1IF
	goto main_master_irupt
	movlb 1
	movlw kI2CCollision
	movwf BANKMASK(i2cerror)
	movlw kI2CUserCallbackState
	call  _i2c_setstate

	main_master_irupt:
	;switch(i2c_status.state)
	call _i2c_getstate
	brw
	return				;kI2CIdleState
	goto sendaddr
	goto senddata
	goto sendcheck
	goto recvdata
	goto receivecheck
	return 				;kI2CUserCallbackState
	goto receivedcheck

	sendaddr:
	movf BANKMASK(bus_address),w
	call _i2c_transmit
	call _i2c_choose_direction
	return

	senddata:
	movlw pack_i2c_states(1, 1, kI2CSendDataState, kI2CSendChecksumState)
	call _i2c_core_transfer
	return

	sendcheck:
	call _i2c_calc_check	;leaves check in w
	call _i2c_transmit   
	movlw kI2CReceivedChecksumState
	call _i2c_setstate
	return

	recvdata:
	call _i2c_master_receivedata
	return

	receivecheck:
	call _i2c_master_receivechecksum
	return

	receivedcheck:
	movlw kI2CUserCallbackState
	call _i2c_setstate
	movf BANKMASK(i2cerror),w
	movlb 4
	bittfsc(SSP1CON2, ACKSTAT)
	movlw kI2CNackReceived
	movlb 1
	movwf BANKMASK(i2cerror)
	return
ENDFUNCTION _i2c_master_interrupt