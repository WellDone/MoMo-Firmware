#include "measure.h"
#include "tdc1000.h"
#include "tdc7200.h"
#include "communication.h"
#include <xc.h>
#include "port.h"

#define kMedianWindowSize	11
#define kScratchSize		100

int32_t measurements[kMedianWindowSize];
int32_t scratch[100];

/*
 *  The TDC1000 has 2 settings for qualifying echos for tof measurements. The echo must:
 *   - exceed a fixed theshold voltage
 *	 - after being amplified by a certain amount
 */

const trigger_conditions triggers[128] = 
{
	{41, 0}, {38, 0}, {41, 1}, {35, 0}, {38, 1}, {41, 2}, {32, 0}, {35, 1}, {38, 2}, {41, 3}, {29, 0}, {32, 1}, {35, 2},
	{38, 3}, {26, 0}, {29, 1}, {32, 2}, {41, 4}, {35, 3}, {23, 0}, {26, 1}, {29, 2}, {38, 4}, {21, 0}, {32, 3}, {20, 0}, 
	{23, 1}, {41, 5}, {26, 2}, {35, 4}, {18, 0}, {29, 3}, {21, 1}, {20, 1}, {38, 5}, {23, 2}, {32, 4}, {15, 0}, {26, 3}, 
	{18, 1}, {21, 2}, {41, 6}, {35, 5}, {20, 2}, {29, 4}, {12, 0}, {23, 3}, {15, 1}, {18, 2}, {38, 6}, {32, 5}, {26, 4}, 
	{21, 3}, {9, 0}, {20, 3}, {12, 1}, {15, 2}, {41, 7}, {35, 6}, {29, 5}, {23, 4}, {18, 3}, {6, 0}, {9, 1}, {12, 2}, 
	{38, 7}, {32, 6}, {21, 4}, {26, 5}, {20, 4}, {15, 3}, {3, 0}, {6, 1}, {9, 2}, {35, 7}, {29, 6}, {18, 4}, {23, 5}, 
	{12, 3}, {0, 0}, {3, 1}, {21, 5}, {6, 2}, {32, 7}, {26, 6}, {15, 4}, {20, 5}, {9, 3}, {0, 1}, {18, 5}, {3, 2}, {29, 7}, 
	{23, 6}, {12, 4}, {6, 3}, {21, 6}, {15, 5}, {0, 2}, {26, 7}, {20, 6}, {9, 4}, {3, 3}, {18, 6}, {12, 5}, {23, 7}, {6, 4}, 
	{0, 3}, {21, 7}, {15, 6}, {9, 5}, {20, 7}, {3, 4}, {18, 7}, {12, 6}, {6, 5}, {0, 4}, {15, 7}, {9, 6}, {3, 5}, {12, 7}, 
	{6, 6}, {0, 5}, {9, 7}, {3, 6}, {6, 7}, {0, 6}, {3, 7}, {0, 7}
}

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
 * Given a situation where the probes are not connected to each other acoustically
 * so there should be no signal, attempt to figure out where the noise floor is.
 */

uint32_t noise_floor()
{

}

/*
 * Make a delta TOF measurement and remove noise with a built-in
 * fixed size median filter
 */

measure_delta_tof(int32_t *out)
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