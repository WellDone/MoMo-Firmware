//gsm_defines.h

#ifndef __gsm_defines_h__
#define __gsm_defines_h__

#define _XTAL_FREQ			4000000

#define MODULEPOWERPIN 	A4

#define GSMPOWERPIN		A5

#define GSMRESETPIN		C3

#define GSMSTATUSPIN	C2

#define SIMDET_POW		A1

#define SIMDET_SENSE	A0


#define set_powerpin(v)	POWERPIN=v

//Useful Macros
#define MAKE_ANALOGR(pin)	{ANS##pin = 1; TRIS##pin = 1;}
#define MAKE_DIGITALR(pin)	ANS##pin = 0
#define SET_DIRECTIONR(pin, val) TRIS##pin = val
#define SET_LEVELR(pin, val)	 R##pin = val
#define PINR(pin)			 R##pin

#define DRIVE_LOWR(pin)		{SET_LEVEL(pin, 0); SET_DIRECTION(pin, 0);}
#define DRIVE_HIR(pin)		{SET_LEVEL(pin, 1); SET_DIRECTION(pin, 0);}

#define MAKE_ANALOG(pin)	MAKE_ANALOGR(pin)
#define MAKE_DIGITAL(pin)	MAKE_DIGITALR(pin)
#define SET_DIRECTION(pin, val) SET_DIRECTIONR(pin, val)
#define SET_LEVEL(pin, val)	 SET_LEVELR(pin, val)
#define PIN(pin)			 PINR(pin)

#define DRIVE_LOW(pin)		DRIVE_LOWR(pin)
#define DRIVE_HI(pin)		DRIVE_HIR(pin)
#endif