#ifndef __timerx_h__
#define __timerx_h__

/*
 * Macros for interacting with the 8 bit Timer 2 type modules in the enhanced midrange 
 * family of PIC microcontrollers.
 */

//Postscaler Values
#define kTMRPostscale1_1 	0b0000
#define kTMRPostscale1_2 	0b0001		
#define kTMRPostscale1_3 	0b0010		
#define kTMRPostscale1_4 	0b0011		
#define kTMRPostscale1_5 	0b0100
#define kTMRPostscale1_6 	0b0101
#define kTMRPostscale1_7 	0b0110
#define kTMRPostscale1_8 	0b0111
#define kTMRPostscale1_9	0b1000
#define kTMRPostscale1_10	0b1001
#define kTMRPostscale1_11	0b1010
#define kTMRPostscale1_12	0b1011
#define kTMRPostscale1_13	0b1100
#define kTMRPostscale1_14	0b1101
#define kTMRPostscale1_15	0b1110
#define kTMRPostscale1_16	0b1111

//Prescaler Values
#define kTMRPrescale_1 		0b00
#define kTMRPrescale_4 		0b01
#define kTMRPrescale_16 	0b10
#define kTMRPrescale_64 	0b11

#define tmrcon_r(n)			T ## n ## CON
#define tmrperiod_r(n)		PR ## n
#define tmrval_r(n)			TMR ## n
#define tmron_r(n)			TMR ## n ## ON
#define tmrflag_r(n)		TMR ## n ## IF
#define tmren_r(n)			TMR ## n ## IE 

#define tmr_con(n)			tmrcon_r(n)
#define tmr_period(n)		tmrperiod_r(n)
#define tmr_value(n)		tmrval_r(n)
#define tmr_intenable(n)	tmren_r(n)
#define tmr_flag(n)			tmrflag_r(n)

#define tmr_config(n, prescale, postscale) 	tmr_con(n) = ((prescale) & 0b11) | (((postscale) & 0b1111) << 3)
#define tmr_setstate(n, on)					tmron_r(n) = (on)
#define tmr_load(n, val) 					tmr_value(n) = (val)
#define tmr_loadperiod(n, val) 				tmr_period(n) = (val)

#endif