//flow_meter.c

#include "flowmeter.h"
#include "flow_defines.h"
#include "mib12_api.h"

void flowmeter_set_enabled(uint8 enabled)
{
	if (enabled)
	{
		FLOWPOWERPIN = 1;
		FLOWPOWERTRIS = 0;
	}
	else
		FLOWPOWERTRIS = 1;
}

void flowmeter_startsampling()
{
	flowmeter_set_enabled(1);

	TMR1ON = 1;
	TMR1GE = 1;
	T1OSCEN = 0;
	TMR1CS0 = 0;
	TMR1CS1 = 1;
	nT1SYNC = 1; //do not sync
	T1GPOL = 1; //active high
	T1GTM  = 0; //do not divide by 2
	T1GSPM = 0; //not single pulse
	T1GSS0 = 0; //use gate pin as src
	T1GSS1 = 0;

	TMR1H = 0;
	TMR1L = 0;
}

void flowmeter_endsampling()
{
	TMR1ON = 0;
	flowmeter_set_enabled(0);
}

/*
 * sampletime should be a watchdog timer timeout value.  The number of 
 * pulses in that 
 */ 

void flowmeter_singlesample(uint8 sampletime)
{
	flowmeter_startsampling();

	WDTCON = sampletime;
	asm("sleep");

	flowmeter_endsampling();
}

void flowmeter_statuscmd()
{
	if (mib_buffer[0])
		flowmeter_startsampling();
	else
		flowmeter_endsampling();

	bus_slave_setreturn(pack_return_status(0,0));
}

void flowmeter_readcmd()
{
	mib_buffer[0] = TMR1L;
	mib_buffer[1] = TMR1H;

	bus_slave_setreturn(pack_return_status(0,2));	
}

void flowmeter_oneshotcmd()
{
	flowmeter_singlesample(mib_buffer[0]);

	flowmeter_readcmd();
}