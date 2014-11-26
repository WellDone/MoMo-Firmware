/*
 * timer.h
 * Generic macros for interfacing with the PIC24 family's timers
 */

#ifndef __timer_h__
#define __timer_h__

#include "bit_utilities.h"

//Internal definitions
#define TCON(timer)			T##timer##CON
#define TCONBits(timer)		T##timer##CONbits
#define TPER(timer)			PR##timer
#define TMR(timer)			TMR##timer
#define TFLAG(timer)		_T##timer##IF
#define TPRI(timer)			_T##timer##IP
#define TEN(timer) 			_T##timer##IE

enum
{
	kTimerPrescale256 = 0b11,
	kTimerPrescale64 = 0b10,
	kTimerPrescale8 = 0b01,
	kTimerPrescale1 = 0b00
};

//Module API
#define timer_start(timer)					SET_BIT(TCON(timer), 15)
#define timer_stop(timer)					CLEAR_BIT(TCON(timer), 15)
#define timer_set_prescaler(timer, scale)	TCONBits(timer).TCKPS = (scale)
#define timer_function_idle(timer, fun)		TCONBits(timer).TSIDL = (fun)
#define timer_load_period(timer, period) 	TPER(timer) = (period)
#define timer_clear(timer) 					TMR(timer) = 0x00

#define timer_int_flag(timer)				TFLAG(timer)
#define timer_int_priority(timer)			TPRI(timer)
#define timer_int_enable(timer)				TEN(timer)

#endif
