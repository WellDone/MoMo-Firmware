#include "pulse.h"
#include "timerx.h"
#include "sample.h"
#include "port.h"
#include "ioc.h"
#include "timer1.h"
#include "ccp.h"
#include <xc.h>

#define kTimer1_HalfSecond 3036

#define MAX_PULSES		20		//Must be less than 256

//Discard all pulses with width shorter than this
#define MIN_PULSE_WIDTH	(125UL*5UL) //5 ms in units of tmr1 counts (8 us)

//Discard all pulses that are closer to eachother than this value.
//This limits the highest flowrate that can be counted.  With the
//Digisavant 8800H (1" flow sensor) there are 65 pulses per liter
//so 25 ms would be (1000/25./65) .6 liters per second, which is a lot.
#define MIN_PULSE_DELAY (125UL*25UL) //25 ms in units of tmr1 counts (8 us)

uint8_t periods;

uint16_t pulse_widths[MAX_PULSES];
uint16_t pulse_intervals[MAX_PULSES];
uint16_t pulse_temp_intervals[MAX_PULSES];
uint16_t pulse_start;
uint16_t last_valid_start;
uint8_t  pulses;
uint8_t  invalid_pulses;

void pulse_readone();

void pulse_sample()
{
	PIN_DIR(PULSE_IN, INPUT);
	set_analog_power(1);

	/*
	 * Count pulses for 500 ms recording the number
	 * of complete pulses (falling edge, low period, rising edge)
	 */

	pulses = 0;
	invalid_pulses = 0;
	pulse_start = 0;
	last_valid_start = 0;
	
	//Use 16 bit timer to capture pulse widths and intervals
	tmr1_config(pack_tmr1_config(kInstructionClock, kPrescaler1_8));
	tmr1_load(kTimer1_HalfSecond);
	tmr1_setstate(kEnabled_Sync);

	//Wait for the pin to go high so that we have a valid signal
	while(PIN(PULSE_IN) == 0 && tmr1_flag == 0)
		;
	
	if (tmr1_flag == 0)
	{
		ioc_flag_b(PULSE_IOC) = 0;
		ioc_detect_falling_b(PULSE_IOC, 1);
		ioc_detect_rising_b(PULSE_IOC, 1);

		//Actual counting happens in interrupt code
		while(tmr1_flag == 0)
			;

		//Disable everything
		ioc_detect_falling_b(PULSE_IOC, 0);
		ioc_detect_rising_b(PULSE_IOC, 0);
	}

	tmr1_setstate(kDisabled);
	set_analog_power(0);
	PIN_DIR(PULSE_IN, OUTPUT);
}

void pulse_falling_edge()
{
	pulse_start = tmr1_get();
}

void pulse_rising_edge()
{
	uint16_t pulse_end = tmr1_get();
	uint16_t temp = pulse_end - pulse_start;

	//Make sure the pulse was a certain width
	if (temp <= MIN_PULSE_WIDTH)
	{
		++invalid_pulses;
		return;
	}

	//Make sure the pulse is delayed by at least a certain
	//amount
	if (pulses > 0)
	{
		temp = pulse_start - last_valid_start;
		if (temp <= MIN_PULSE_DELAY)
		{
			++invalid_pulses;
			return;
		}
	}

	if (pulses < MAX_PULSES)
	{
		pulse_widths[pulses] = pulse_end - pulse_start;

		if (pulses > 0)
			pulse_intervals[pulses-1] = pulse_start - last_valid_start;

		++pulses;
	}

	last_valid_start = pulse_start;
}

uint8_t pulse_find_lowest_interval()
{
	uint8_t i;
	uint8_t lowest_i = 0;
	uint16_t lowest = 65535;

	for (i=0; i<(pulses-1); ++i)
	{
		if (pulse_temp_intervals[i] < lowest)
		{
			lowest = pulse_temp_intervals[i];
			lowest_i = i;
		}
	}

	return lowest_i;
}

/*
 * Destructively compute the median value of the list of pulse intervals
 * Use median instead of mean so that we're not sensitive to outliers.
 * This could be optimized by using the Selection Algorithm rather than
 * the slow one here, but since pulse_intervals is limited to < 20 entries
 * it should be fine.
 */

uint16_t pulse_median_interval()
{
	uint8_t i;
	uint8_t midpoint;

	if (pulses < 2)
		return 0;

	if (pulses == 2)
		return pulse_intervals[0];

	midpoint = (pulses-1) >> 1;

	//Make a temporary copy that we can destructively modify to find the median
	for (i=0; i<pulses; ++i)
		pulse_temp_intervals[i] = pulse_intervals[i];

	for (i=0; i<midpoint; ++i)
	{
		uint8_t low_i = pulse_find_lowest_interval();
		pulse_temp_intervals[low_i] = 65535;
	}

	i = pulse_find_lowest_interval();
	return pulse_temp_intervals[i];
}

uint16_t pulse_count()
{
	return pulses;
}

uint16_t pulse_invalid_count()
{
	return invalid_pulses;
}

uint16_t pulse_width(uint8_t pulse)
{
	if (pulse >= pulses)
		return -1;

	return pulse_widths[pulse];
}

uint16_t pulse_interval(uint8_t pulse)
{
	if (pulse >= (pulses-1))
		return -1;

	return pulse_intervals[pulse];
}