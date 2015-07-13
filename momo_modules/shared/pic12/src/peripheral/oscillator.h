/*
 * oscillator.h
 * Macros for controlling the internal oscillator settings of the PIC12 and PIC16 series
 */

#ifndef __oscillator_h__
#define __oscillator_h__

#include <xc.h>

//Internal Oscillator States
#define k16MhzHFOsc		0b1111
#define k8MhzHFOsc		0b1110
#define k4MhzHFOsc		0b1101
#define k2MhzHFOsc		0b1100
#define k1MhzHFOsc 		0b1011

//Other OSCCON bits of note
#define kSystemClockInternalSelection	(0b10)

#define set_hf_speed(val)	OSCCON = (((val) & 0b1111) << 3) | kSystemClockInternalSelection
#define set_32mhz_speed()	OSCCON = (((k8MhzHFOsc) & 0b1111) << 3) | kSystemClockInternalSelection | (1 << 7)
#define set_osccon(val)	 OSCCON = (val)


#endif
