//watchdog.h

#ifndef __watchdog_h__
#define __watchdog_h__

#include "platform.h"

#define clearwdt()		asm("clrwdt")
#define wdt_enable()	SWDTEN = 1
#define wdt_disable()	SWDTEN = 0

enum
{
	k1SecondTimeout  = 0b010100,
	k4SecondTimeout  = 0b011000,
	k8SecondTimeout  = 0b011010,
	k16SecondTimeout = 0b011100
};

void wdt_pushenabled();
void wdt_popenabled();
void wdt_settimeout(uint8 timeout);

#endif