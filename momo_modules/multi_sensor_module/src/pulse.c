#include "pulse.h"
#include "timerx.h"
#include "sample.h"
#include "port.h"
#include "ioc.h"
#include "ccp.h"
#include <xc.h>


uint16_t counter;
uint16_t periods;

#define k100msTimeConstant 	53036

void pulse_readone();

void pulse_sample()
{
	PIN_DIR(PULSE_IN, INPUT);
	set_analog_power(1);

	/*
	 * Count pulses for 0.1 seconds
	 */

	tmr_config(2, kTMRPrescale_64, kTMRPostscale1_16);
	tmr_loadperiod(2, 98);
	tmr_load(2, 0);
	tmr_flag(2) = 0;
	tmr_setstate(2, 1);
	
	//Wait for the pin to go high so that we have a valid signal
	while(PIN(PULSE_IN) == 0 && tmr_flag(2) == 0)
		;
	
	if (tmr_flag(2) == 0)
	{
		ioc_flag_b(PULSE_IOC) = 0;
		ioc_detect_falling_b(PULSE_IOC, 1);

		while(tmr_flag(2) == 0)
			;

		//Disable everything
		ioc_detect_falling_b(PULSE_IOC, 0);
	}

	tmr_setstate(2, 0);
	set_analog_power(0);
	PIN_DIR(PULSE_IN, OUTPUT);

	++periods;
}

uint16_t pulse_count()
{
	return counter;
}