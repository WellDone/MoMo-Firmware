//gsm_defines.h

#ifndef __gsm_defines_h__
#define __gsm_defines_h__

#define POWERPIN 		RA4
#define POWERTRIS 		TRISA4

#define GSMPOWERPIN		LATA5
#define GSMPOWERTRIS	TRISA5
#define GSMRESETPIN		LATC3
#define GSMRESETTRIS	TRISC3

#define GSMSTATUSPIN	RC2
#define GSMSTATUSTRIS	TRISC2


#define set_powerpin(v)	POWERPIN=v

#endif