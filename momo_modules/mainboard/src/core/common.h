#ifndef __common_h__
#define __common_h__

#include "pic24.h"

#define SENSOR_INTERRUPT_BIT    IFS1bits.INT2IF
#define RTC_INTERRUPT_BIT       IFS3bits.RTCIF

#define BATTERY_VOLTAGE_LATCH	_LATA1
#define BATTERY_VOLTAGE_TRIS	_TRISA1
#define BATTERY_VOLTAGE_DIGITAL	_PCFG1

#define SOLAR_VOLTAGE_LATCH 	_LATA3
#define SOLAR_VOLTAGE_TRIS		_TRISA3
#define SOLAR_VOLTAGE_DIGITAL	_PCFG5
#define SOLAR_VOLTAGE_OD		_ODA3

#define DEBUG_UART              U2
#define GSM_UART                U1

#endif
