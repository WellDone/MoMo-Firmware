;i2c_utilities.as
;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>

GLOBAL _mib_state, _i2c_status,_i2c_choose_direction

PSECT text_i2c_utils,local,class=CODE,delta=2

;Set the i2c_status state to either kI2CSendDataState or kI2CReceiveDataState based on whether
;bit 1 of address is set
;i2c_status.state = (((i2c_msg->address) & 0x01) == 0) ? kI2CSendDataState : kI2CReceiveDataState;
;mib_state and i2c_status must be in bank1 and must not change size
_i2c_choose_direction:
	movlb 1
	movlw 0b10000000
	andwf BANKMASK(_i2c_status),f 		;clear state part of i2cstatus (lower 7 bits)
	btfss BANKMASK(_mib_state+3),0 		;test if bit 1 of address is set
	goto setsenddata
	movlw 4
	goto setstate
	setsenddata:
	movlw 2
	setstate:
	iorwf BANKMASK(_i2c_status),f		;set receive data
	return