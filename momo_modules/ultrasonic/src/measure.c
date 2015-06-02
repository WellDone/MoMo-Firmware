#include "measure.h"
#include "tdc1000.h"
#include "tdc7200.h"
#include "communication.h"
#include <xc.h>
#include "port.h"

#define kMedianWindowSize	11

int32_t measurements[kMedianWindowSize];

void set_parameters(uint8_t gain, uint8_t threshold, uint8_t pulses, uint16_t mask)
{
	uint8_t lna = (gain < 20);

	if (gain >= 20)
		gain -= 20;

	gain /= 3;

	tdc1000_setgain(gain, lna, threshold);
	tdc1000_setexcitation(pulses, 0);
	tdc7200_setstopmask(mask);
}

/*
 * Make a delta TOF measurement and remove noise with a built-in
 * fixed size median filter
 */

int32_t measure_delta_tof()
{
	int32_t tof1, tof2;
	int32_t delta = 0;
	uint8_t i;

	tdc7200_setaverages(0);
	tdc1000_prepare_deltatof(0);

	enable_power();
	
	for (i=0; i<kMedianWindowSize; ++i)
	{
		take_measurement();

		tof1 = tdc7200_tof(0, 0);

		tdc7200_trigger();
		while (PIN(INT7200) && PIN(ERR1000))
			;

		tof2 = tdc7200_tof(0, 0);

		measurements[i] = tof2 - tof1;
	}

	disable_power();

	//Median filter 
	sort_measurements();
	return measurements[kMedianWindowSize/2];
}

uint8_t take_measurement()
{
	uint8_t retval;

	retval = tdc1000_push();
	if (retval == 0)
	{	
		retval = tdc7200_start();
		if (retval == 0)
		{
			while (PIN(INT7200) && PIN(ERR1000))
				;
		}
	}

	return retval;
}

/*
 * Insertion sort of the measurment array from low to high
 */

void sort_measurements()
{
	uint8_t i, j;
	int32_t temp;

	for (i=1; i<kMedianWindowSize; ++i)
	{
		j = i;

		while (j > 0 && measurements[j] < measurements[j-1])
		{
			temp = measurements[j];
			measurements[j] = measurements[j-1];
			measurements[j-1] = temp;

			--j;
		}
	}
}