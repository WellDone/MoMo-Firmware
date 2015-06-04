#include "measure.h"
#include "tdc1000.h"
#include "tdc7200.h"
#include "communication.h"
#include <xc.h>
#include <stddef.h>
#include "port.h"

#define kMedianWindowSize			11
#define kNumberOfTriggerSettings	128
#define kMaximumTOFDelta			500000LL

int32_t measurements[kMedianWindowSize];

/*
 *  The TDC1000 has 2 settings for qualifying echos for tof measurements. The echo must:
 *   - exceed a fixed theshold voltage
 *	 - after being amplified by a certain amount of decibels
 *
 *  This array contains an ordered list of all of the corresponding voltages that can be achieved
 *  by combining a gain setting with a threshold voltage.  The gains are listed in db and the thresholds
 *  using their threshold index.
 */

const trigger_condition triggers[kNumberOfTriggerSettings] = 
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
};

//Trigger levels in nanovolts
const uint32_t trigger_levels[8] = {35000000, 50000000, 75000000, 125000000, 220000000, 410000000, 775000000, 1500000000};

//Gain levels in V/V encoded as fixed point .3 decimal digits (so * 1000)
//So integer division between a trigger_level and a gain_level produces a trigger voltage in microvolts
const uint32_t gain_levels[16] = {1000, 1412, 1995, 2818, 3981, 5623, 7943, 10000, 11220, 14125, 19952, 28183, 39810, 56234, 79432, 112201};

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
 *
 * Find the noise floor by starting with the highest trigger voltage (highest threshold with lowest gain)
 * and then decreasing the trigger until the device triggers without any signal present, indicating that
 * noise is causing the triggering.
 *
 * TODO: 
 *	- 	We can use a mask with a large mask value to make this work even with a signal present, at the cost
 *  	of a longer measurement time.  This would require leaving the TDC1000 on forever without a timeout.
 *
 *  - 	We should also not hardcode the number of pulses here so that we can take that from a configuration
 *		setting.
 */

uint8_t noise_floor_index()
{
	int8_t i = kNumberOfTriggerSettings-1;
	trigger_condition trigger;
	uint8_t gain_index = 0;

	for (i=kNumberOfTriggerSettings-1; i>=0; --i)
	{
		UltrasoundError err;

		trigger = triggers[i];
		set_parameters(trigger.gain, trigger.threshold, 8, 64);	//FIXME: Don't hardcode the number of pulses and the mask here, take it from a previous config

		err = measure_delta_tof(NULL);
		if (err != kNoSignalError)
			break;
	}

	return i;
}

uint32_t noise_floor_voltage()
{
	uint8_t index = noise_floor_index();	
	trigger_condition trigger = triggers[index];
	uint8_t gain_index = 0;

	//Figure out the index that corresponds to this gain setting
	if (trigger.gain > 20)
	{
		trigger.gain -= 20;
		gain_index = 8;
	}

	gain_index += trigger.gain / 3;

	return trigger_levels[trigger.threshold] / gain_levels[gain_index];
}

/*
 * Find the combination of trigger voltage and gain that minimizes the standard
 * deviation of a delta TOF measurement given the stop mask that we have
 */

trigger_condition find_optimal_settings(uint16_t mask)
{

}

/*
 * Make a delta TOF measurement and remove noise with a built-in
 * fixed size median filter
 */

UltrasoundError measure_delta_tof(int32_t *out)
{
	uint8_t i;
	int32_t tof1, tof2;
	int32_t delta = 0;
	uint8_t error_code = kNoUltrasoundError;

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

		if (tof1 == 0 && tof2 == 0)
		{
			error_code = kNoSignalError;
			break;
		}
		else if (tof1 == 0 || tof2 == 0)
		{
			error_code = kWeakSignalError;
			break;
		}
		else if ((tof2 - tof1) > kMaximumTOFDelta || (tof2 - tof1) < -kMaximumTOFDelta)
		{
			error_code = kHighNoiseError;
			break;
		}
	}

	disable_power();

	if (error_code != kNoUltrasoundError)
		return error_code;

	if (out == NULL)
		return kNoUltrasoundError;

	//Median filter and output the result
	sort_measurements();
	*out = measurements[kMedianWindowSize/2];

	return kNoUltrasoundError;
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