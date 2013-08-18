//app_main.c

#include "platform.h"
#include "protocol.h"
#include "mib12_api.h"
#include "watchdog.h"
#include "flow_defines.h"

#define _XTAL_FREQ			4000000

void task(void)
{
	wdt_disable();


	while (1)
	{	
		__delay_ms(250);
	}
}

void interrupt_handler(void)
{
}

void initialize(void)
{	
	FLOWPOWERTRIS = 1;
	FLOWDATATRIS = 1;

	T1GSEL = 0; //data pin is on RA4

	//data pin is open collector so enable weak pull-up
	nWPUEN = 0;
	WPUA4 = 1;	//weak pull-up on data pin
	WPUA0 = 0;	//no weak pull-up on other pins
	WPUA1 = 0;
	WPUA2 = 0;
}

/*
 * Do not use, required to be defined by startup code that xc8 apparently cannot *NOT* link in even though it is optimized away later.
 */	
void main()
{
	
}