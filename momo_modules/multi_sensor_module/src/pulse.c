#include "pulse.h"
#include "timer1.h"
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

	tmr1_config(pack_tmr1_config(kInstructionClock, kPrescaler1_8));
	tmr1_load(k100msTimeConstant);
	tmr1_setstate(kEnabled_Sync);
	
	//Wait for the pin to go high so that we have a valid signal
	while(PIN(PULSE_IN) == 0 && TMR1IF == 0)
		;
	
	if (TMR1IF == 0)
	{
		ioc_flag_b(PULSE_IOC) = 0;
		ioc_detect_falling_b(PULSE_IOC, 1);

		while(TMR1IF == 0)
			;

		//Disable everything
		ioc_detect_falling_b(PULSE_IOC, 0);
	}

	tmr1_setstate(kDisabled);
	set_analog_power(0);
	PIN_DIR(PULSE_IN, OUTPUT);

	++periods;
}

uint16_t pulse_count()
{
	return counter;
}