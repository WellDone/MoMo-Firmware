//common.h

#ifndef __common_h__
#define __common_h__

#include <p24Fxxxx.h>
#include <GenericTypeDefs.h>
#define CLOCKSPEED      8000000L //F_osc / 2 (the instruction clock frequency)

typedef unsigned short bool;
#define false 0;
#define true 1;

#define CLEAR_BIT(field, bitnum) ((field) &= ~(1<<bitnum))
#define SET_BIT(field,bitnum)    ((field) |= 1<<bitnum)
#define TOGGLE_BIT(field,bitnum) ((field) ^= 1<<bitnum)

//For this macro, field should be an unsigned variable because of the the right shift operator
#define BIT_TEST(field, bitnum)  ((field & (1 << bitnum)) >> bitnum)

#define SENSOR_INTERRUPT_BIT    IFS1bits.INT2IF
#define RTC_INTERRUPT_BIT       IFS3bits.RTCIF

#define BATTERY_VOLTAGE_LATCH	_LATA1
#define BATTERY_VOLTAGE_TRIS	_TRISA1
#define BATTERY_VOLTAGE_DIGITAL	_PCFG1

#define SOLAR_VOLTAGE_LATCH 	_LATA3
#define SOLAR_VOLTAGE_TRIS		_TRISA3
#define SOLAR_VOLTAGE_DIGITAL	_PCFG5
#define SOLAR_VOLTAGE_OD		_ODA3

// UART
typedef enum
{
    U1 = 0,
    U2 = 1
} UARTPort;

#define DEBUG_UART              U2
#define GSM_UART                U1

#define print( msg ) sends( DEBUG_UART, msg )

// Assembly helpers
extern void asm_reset();
extern void asm_sleep();

#define uninterruptible_start() __builtin_disi(0x3FFF)
#define uninterruptible_end()	__builtin_disi(0x0000)

#endif
