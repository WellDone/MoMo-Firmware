#ifndef __ultrasound_h__
#define __ultrasound_h__

#include <stdint.h>
#include <xc.h>

void enable_power();
void disable_power();
void init_spi();

//TDC7200 interface
uint8_t 	tdc7200_read8(uint8_t address);
void 		tdc7200_write8(uint8_t address, uint8_t value);
uint32_t 	tdc7200_read24(uint8_t address);

//TDC1000 interface
void 		tdc1000_write8(uint8_t address, uint8_t value);
uint8_t 	tdc1000_read8(uint8_t address);

#define 	kTDC7200AddressMask 		0b00111111
#define 	kTDC1000AddressMask			0b00111111

enum
{
	kTDC7200_Config1Reg = 0x00,
	kTDC7200_Config2Reg,
	kTDC7200_INTStatusReg,
	kTDC7200_INTMaskReg,
	kTDC7200_CoarseOverflowHigh,
	kTDC7200_CoarseOverflowLow
	//... others
};

enum
{
	kTDC1000_Config0Reg = 0x00,
	kTDC1000_Config1Reg,
	kTDC1000_Config2Reg,
	kTDC1000_Config3Reg,
	kTDC1000_Config4Reg,
	kTDC1000_TOF_1Reg,
	kTDC1000_TOF_0Reg,
	kTDC1000_ErrorFlagsReg,
	kTDC1000_TimeoutReg,
	kTDC1000_ClockRateReg
};

#endif
