#include "pulse.h"
#include "timer1.h"
#include "sample.h"
#include "sensor_defines.h"
#include "port.h"
#include <xc.h>


uint16_t counter;

void pulse_readone();

void pulse_sample()
{
	counter = 0;

	set_analog_power(1);

	tmr1_config(pack_tmr1_config(kInstructionClock, kPrescaler1_8));
	tmr1_load(kHalfSecondConstant);
	tmr1_setstate(kEnabled_Sync);

	while(TMR1IF == 0)
		pulse_readone();

	set_analog_power(0);
}

void pulse_readone()
{
	while(PIN(PULSE_IN) == 1 && TMR1IF == 0)
		;
	
	if (TMR1IF == 1)
		return;

	while(PIN(PULSE_IN) == 0 && TMR1IF == 0)
		;

	if (TMR1IF == 1)
		return;

	++counter;
}

uint16_t pulse_count()
{
	return counter;
}