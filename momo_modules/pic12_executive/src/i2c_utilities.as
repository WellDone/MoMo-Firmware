;i2c_utilities.as
;Assembly routines for dealing with I2C things in an efficient way
;very quickly and with minimal code overhead


#include <xc.inc>
#include "i2c_defines.h"
#include "asm_locations.h"

ASM_INCLUDE_GLOBALS()

GLOBAL _i2c_enable, _i2c_loadbuffer, _i2c_incptr, _i2c_disable

PSECT text_i2c_utils,local,class=CODE,delta=2

#define clearbit(reg, id)	bcf reg&0x7F, reg##_##id##_POSN
#define setbit(reg, id)		bsf reg&0x7F, reg##_##id##_POSN
#define bittfsc(reg, id)	btfsc reg&0x7F, reg##_##id##_POSN

;_i2c_set_mastermode
;enable or disable mastermode according to w
;w = 1 means enable
;w = 0 means disable
BEGINFUNCTION _i2c_set_master_mode
	movwf FSR1H
	
	movlb 1 
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

	;Load in the slavea addressa and left shift it since it's a 7-bit address
	banksel slave_address
	lslf BANKMASK(slave_address),w
	movlb 	4
	btfsc FSR1H,0
		movlw  0x09	 	;in master mode set baud to 100 khz

	movwf BANKMASK(SSP1ADD)

	movlb 0
	bcf SSP1IF
	movlb 1
	btfss FSR1H,0 		;only enable interrupts in slave mode, master is synchronous
		bsf SSP1IE
	return
ENDFUNCTION _i2c_set_master_mode

;Enable the i2c interface and configure it for mib use
;put it in slave mode
BEGINFUNCTION _i2c_enable
	banksel slave_address
	movwf BANKMASK(slave_address)

	movlb 1
	bsf SDATRIS
	bsf SCLTRIS

	movlb 4
	movlw 0xff & kI2CFlagMask
	movwf BANKMASK(SSP1STAT)
	movlw 0
	call _i2c_set_master_mode
	
	movlb 4
	movf BANKMASK(SSP1BUF),w
	return
ENDFUNCTION _i2c_enable

;Disable the I2C interface and disable any inturrupts that
;it has enabled
BEGINFUNCTION _i2c_disable
	banksel PIE1
	bcf SSP1IE

	banksel SSP1CON2
	bcf BANKMASK(SSP1CON2), SSP1CON2_GCEN_POSN
	bcf BANKMASK(SSP1CON1), SSP1CON1_SSPEN_POSN
	return
ENDFUNCTION _i2c_disable