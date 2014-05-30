#ifndef __timer1_h__
#define __timer1_h__

#include "peripheral_def.h"
#include <stdint.h>
#include <xc.h>

//Clock Sources
enum
{
	kCapSenseOscilator = 0b11,
	kPinOrOscillator = 0b10,
	kSystemClock = 0b01,
	kInstructionClock = 0b00
};

//Prescaler
enum
{
	kPrescaler1_8 = 0b11,
	kPrescaler1_4 = 0b10,
	kPrescaler1_2 = 0b01,
	kPrescaler1_1 = 0b00
};

#define pack_1(val, shift)			((val&0b1) << shift)
#define pack_2(val, shift)			((val&0b11) << shift)

#define pack_tmr1_config(clock, prescaler)	(pack_2(clock, 6) | pack_2(prescaler, 4))

#define kHalfSecondConstant 3036UL

#define tmr1_load( value) \
	TMR1L = value & 0xFF; \
	TMR1H = (value >> 8);

void tmr1_config(uint8_t config);
//void tmr1_load(uint8_t high, uint8_t low);
void tmr1_setstate(PeripheralState state);

#endif