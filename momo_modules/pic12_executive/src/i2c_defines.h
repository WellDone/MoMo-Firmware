//i2c_defines.h
/*
 * All i2c constant values for sharing between c and asm code.
 */

#ifndef __i2c_defines_h__
#define __i2c_defines_h__

#ifdef __PIC16LF1823__
#define SDAPIN		RC1	
#define SCLPIN		RC0

#define I2CPORT		PORTC
#define I2CMASK 	0b00000011

#define SDATRIS		TRISC1
#define SCLTRIS		TRISC0
#define ALARM		A2

#elif  __PIC12LF1822__
#define SDAPIN		RA2	
#define SCLPIN		RA1

#define I2CPORT		PORTA
#define I2CMASK 	0b00000110

#define SDATRIS		TRISA2
#define SCLTRIS		TRISA1
#define ALARM		A5

#elif  __PIC16LF1847__
#define SDAPIN		RB1
#define SCLPIN		RB4

#define I2CPORT		PORTB
#define I2CMASK 	0b00010010

#define SDATRIS 	TRISB1
#define SCLTRIS		TRISB4
#define ALARM		B0
#else
#error Unsupported Platform, need to know i2c pin locations.
#endif

#define kI2CIdleState 				0
#define kI2CSendAddressState 		1
#define kI2CSendDataState 			2
#define kI2CReceiveDataState 		3
#define kI2CUserCallbackState 		4

#define kI2CStateMask				0b111
#define kI2CStatusMask				~(kI2cStateMask)
#define kI2CErrorMask				(0b11 << 6)

#define kI2CNoError 				(0 << 6)
#define kI2CInvalidChecksum 		(1 << 6)
#define kI2CNackReceived 			(2 << 6)
#define kI2CCollision 				(3 << 6)

#define kDisableI2CSlewControlFlag 	(1 << 7)
#define kSMBusLevelCompliantFlag 	(1 << 6)

#define kI2CFlagMask (kDisableI2CSlewControlFlag | kSMBusLevelCompliantFlag)

//both set CKP and SSPEN
#define kI2CMasterMode 				0b00111000            //set to master mode (0b1000)
#define kI2CSlaveMode 		        0b00110110            //set to slave mode (0b0110)


#endif