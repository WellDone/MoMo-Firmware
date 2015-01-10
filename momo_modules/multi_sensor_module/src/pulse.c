#include "pulse.h"
#include "timer1.h"
#include "sample.h"
#include "port.h"
#include "ioc.h"
#include "ccp.h"
#include <xc.h>


uint16_t counter;
uint16_t first_time;
uint16_t second_time;

void pulse_readone();

void pulse_sample()
{
	counter = 0;
	first_time = 0;
	second_time = 0;

	set_analog_power(1);

	/*
	 * TODO, only look for say .1 seconds for 1 pulse
	 * and then reload for .5 seconds.
	 */ 
	tmr1_config(pack_tmr1_config(kInstructionClock, kPrescaler1_8));
	tmr1_load(kHalfSecondConstant);
	tmr1_setstate(kEnabled_Sync);

	//Wait for the pin to go high  and then low after giving power to the 
	//turbine sensor to look for evidence of a pulse
	while(PIN(PULSE_IN) == 0 && TMR1IF == 0)
		;

	while(PIN(PULSE_IN) == 1 && TMR1IF == 0)
		;

	if (TMR1IF == 0)
	{
		first_time = tmr1_get();

		ioc_flag_b(PULSE_IOC) = 0;
		ioc_detect_rising_b(PULSE_IOC, 1);

		ccp_set_if(1, 0);
		ccp_config(1, kCCPCapture4thRisingEdge);
		ccp_set_ie(1, 1);
	}

	while(TMR1IF == 0)
		;

	//Disable everything
	ccp_set_ie(1, 0);
	ccp_off(1);
	ioc_detect_rising_b(PULSE_IOC, 0);
	set_analog_power(0);
}

uint16_t pulse_count()
{
	return counter;
}