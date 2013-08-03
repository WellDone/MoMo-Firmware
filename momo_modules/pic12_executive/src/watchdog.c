//watchdog.c

#include "watchdog.h"
#include "bootloader.h"

extern MIBExecutiveStatus status;

void wdt_settimeout(uint8 timeout)
{
	WDTCON = timeout;
}