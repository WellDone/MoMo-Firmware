//gsm_defines.h

#ifndef __gsm_defines_h__
#define __gsm_defines_h__

#include "port.h"

#define _XTAL_FREQ			4000000

#ifndef MODULEPOWERPIN
#error You must define MODULEPOWERPIN in moudle_settings.json
#endif

#ifndef GSMPOWERPIN
#error You must define GSMPOWERPIN in moudle_settings.json
#endif

#ifndef GSMRESETPIN
#error You must define GSMRESETPIN in moudle_settings.json
#endif

#ifndef GSMSTATUSPIN
#error You must define GSMSTATUSPIN in moudle_settings.json
#endif

#ifndef SIMDET_POW
#error You must define SIMDET_POW in moudle_settings.json
#endif

#ifndef SIMDET_SENSE
#error You must define SIMDET_SENSE in moudle_settings.json
#endif

#define set_powerpin(v)	POWERPIN=v

#define DRIVE_LOWR(pin)		{PIN_SET(pin, 0); PIN_DIR(pin, 0);}
#define DRIVE_HIR(pin)		{PIN_SET(pin, 1); PIN_DIR(pin, 0);}

#define DRIVE_LOW(pin)		DRIVE_LOWR(pin)
#define DRIVE_HI(pin)		DRIVE_HIR(pin)
#endif