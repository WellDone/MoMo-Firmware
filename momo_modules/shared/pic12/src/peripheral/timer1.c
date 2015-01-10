#include "timer1.h"
#include <xc.h>

void tmr1_config(uint8_t config)
{
	T1CON = config;
}

void tmr1_setstate(PeripheralState state)
{
	TMR1IE = 0;

	if (state == kDisabled)
		TMR1ON = 0;
	else
	{
		TMR1IF = 0;

		if (state == kEnabled_Async)
			TMR1IE = 1;

		TMR1ON = 1;
	}
}