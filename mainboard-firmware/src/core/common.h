//common.h

#ifndef __common_h__
#define __common_h__

#define CLOCKSPEED      8000000L //F_osc / 2 (the instruction clock frequency)

#define CLEAR_BIT(field, bitnum) ((field) &= ~(1<<bitnum))
#define SET_BIT(field,bitnum)    ((field) |= 1<<bitnum)
#define TOGGLE_BIT(field,bitnum) ((field) ^= 1<<bitnum)

//For this macro, field should be an unsigned variable because of the the right shift operator
#define BIT_TEST(field, bitnum)  ((field & (1 << bitnum)) >> bitnum)

// Input/Output and Interrupt PIN definitions
#define _GSM_MODULE_POWER       _LATA0
#define _GSM_MODULE_POWER_ON()  _GSM_MODULE_POWER = 1
#define _GSM_MODULE_POWER_OFF() _GSM_MODULE_POWER = 0

#define _GSM_MODULE_ON          _LATA2

#define SENSOR_INTERRUPT_BIT    IFS1bits.INT2IF
#define RTC_INTERRUPT_BIT       IFS3bits.RTCIF

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
