//gsm_defines.h

#ifndef __gsm_defines_h__
#define __gsm_defines_h__

#define POWERPIN 		RA2
#define POWERTRIS 		TRISA2

#define GSMPOWERPIN		LATC5
#define GSMPOWERTRIS	TRISC5
#define GSMRESETPIN		LATC4
#define GSMRESETTRIS	TRISC4

#define GSMSTATUSPIN	RA5


#define set_powerpin(v)	POWERPIN=v

#endif