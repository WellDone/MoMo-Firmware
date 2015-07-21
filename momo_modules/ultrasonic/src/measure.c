#include "measure.h"
#include "tdc1000.h"
#include "tdc7200.h"
#include "communication.h"
#include "mib12_api.h"
#include <xc.h>
#include <stddef.h>
#include <stdint.h>
#include "port.h"
#include "timerx.h"

//int32_t measurements[kMedianWindowSize];


int32_t tof1[kNumTOFPulses];
int32_t tof2[kNumTOFPulses];

int16_t measurements[kMeasurementBufferSize];

int32_t 	delta_tof_accum;
uint16_t 	num_measurements;
uint16_t	desired_measurements;
uint16_t 	desired_shift;

uint8_t 	g_gain;
uint8_t 	g_pulses;
uint8_t		g_threshold;
uint16_t 	g_mask;

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
//const uint32_t trigger_levels[8] = {35000000ULL, 50000000ULL, 75000000ULL, 125000000ULL, 220000000ULL, 410000000ULL, 775000000ULL, 1500000000ULL};

//Gain levels in V/V encoded as fixed point .3 decimal digits (so * 1000)
//So integer division between a trigger_level and a gain_level produces a trigger voltage in microvolts
//const uint32_t gain_levels[16] = {1000ULL, 1412ULL, 1995ULL, 2818ULL, 3981ULL, 5623ULL, 7943ULL, 10000ULL, 11220ULL, 14125ULL, 19952ULL, 28183ULL, 39810ULL, 56234ULL, 79432ULL, 112201ULL};

void initialize_parameters()
{
	uint8_t lna, gain;
	uint8_t stops;

	g_pulses		= 5;
	g_mask 			= 8*8;
	g_threshold 	= 1;
	g_gain 			= 30;

	desired_shift = 7;
	desired_measurements = 128;

	gain = g_gain;
	uint8_t lna = (gain < 20);

	if (gain >= 20)
		gain -= 20;

	gain /= 3;

	tdc1000_setgain(gain, lna, g_threshold);
	tdc1000_setexcitation(g_pulses, g_pulses);
	tdc7200_setstopmask(g_mask);
	tdc1000_setstarttime(25);
	tdc7200_setstops(0b100);
}

void set_gain_index(uint8_t index)
{
	set_gain(triggers[index].gain, triggers[index].threshold);
}

void set_gain(uint8_t gain, uint8_t threshold)
{
	uint8_t lna = (gain < 20)
;
	g_gain = gain;
	g_threshold = threshold;

	if (gain >= 20)
		gain -= 20;

	gain /= 3;

	tdc1000_setgain(gain, lna, threshold);
}

void set_parameters(uint8_t gain, uint8_t threshold, uint8_t pulses, uint16_t mask)
{
	set_gain(gain, threshold);

	g_pulses = pulses;
	g_mask = mask;

	tdc1000_setexcitation(pulses, pulses);
	tdc7200_setstopmask(mask);
}

void fill_parameters()
{
	mib_buffer[0] = g_gain;
	mib_buffer[1] = 0;
	mib_buffer[2] = g_threshold;
	mib_buffer[3] = 0;
	mib_buffer[4] = g_pulses;
	mib_buffer[5] = 0;
}

UltrasoundError measure_delta_tof(uint8_t control_power, uint8_t average_bits)
{
	uint8_t error_code = kNoUltrasoundError;
	uint8_t i;	

	if (control_power)
		enable_power();

	LATCH(CHSEL) = 0;
	
	tdc7200_trigger();
	error_code = wait_for_measurement(200);
	
	if (error_code == kNoUltrasoundError)
	{
		tdc7200_readresults();
		
		for (i=0; i<kNumTOFPulses; ++i)
			tof1[i] = tdc7200_tof(i, average_bits);

		LATCH(CHSEL) = 1;

		tdc1000_reset();
		tdc7200_trigger();
		error_code = wait_for_measurement(200);

		if (error_code == kNoUltrasoundError)
		{
			tdc7200_readresults();
			for (i=0; i<kNumTOFPulses; ++i)
				tof2[i] = tdc7200_tof(i, average_bits);
		}
	}

	if (control_power)
		disable_power();

	return error_code;
}

/*
 * Average 1 << bits measurements quickly to produce an measurement of the delta TOF
 * Do not robustly look for misaligned pulses on the send and receive side.
 * bits must be between 0 and 7 inclusive for between 1 and 128 measurements.  For each
 * measurement, average the delta TOF of the first 4 stops so we actually do 2^(bits + 2)
 * measurements.
 */

UltrasoundError fast_accumulate_delta_tof(uint8_t bits)
{
	UltrasoundError err;

	tdc1000_prepare_deltatof(bits);
	tdc7200_setaverages(bits);

	enable_power();	
	err = measure_delta_tof(0, bits);
	disable_power();

	if (err != kNoUltrasoundError)
		return err;

	delta_tof_accum = tof1[1] - tof2[1];
	//delta_tof_accum += ();
	//delta_tof_accum += (tof1[1] - tof2[1]);
	//delta_tof_accum += (tof1[2] - tof2[2]);
	//delta_tof_accum += (tof1[3] - tof2[3]);

	//delta_tof_accum >>= 2;

	return kNoUltrasoundError;
}

UltrasoundError accumulate_delta_tof(uint8_t bits)
{
	uint8_t num_periods = 0;

	desired_measurements = (uint16_t)1 << (uint16_t)bits;
	tdc1000_prepare_deltatof(bits);
	tdc7200_setaverages(0);

	enable_power();

	delta_tof_accum = 0;
	num_measurements = 0;
	//FIXME: Add bit shift count here;

	tmr_config(ACCUM_TIMER, kTMRPrescale_64, kTMRPostscale1_16);
	tmr_intenable(ACCUM_TIMER) = 0;
	tmr_flag(ACCUM_TIMER) = 0;
	tmr_loadperiod(ACCUM_TIMER, 255);
	tmr_load(ACCUM_TIMER, 0);
	tmr_setstate(ACCUM_TIMER, 1);

	do
	{
		if (tmr_flag(ACCUM_TIMER))
		{
			num_periods += 1;

			if (num_periods == 16)
				break;

			tmr_flag(ACCUM_TIMER) = 0;
			tmr_loadperiod(ACCUM_TIMER, 255);
			tmr_load(ACCUM_TIMER, 0);
			tmr_setstate(ACCUM_TIMER, 1);
		}

		measure_delta_tof(0, 0);
	} while (accumulate_samples() == 0);

	tmr_setstate(ACCUM_TIMER, 0);
	disable_power();

	if (tmr_flag(ACCUM_TIMER))
		return kTimeoutError;

	delta_tof_accum >>= bits;

	return kNoUltrasoundError;
}

/*
 * Attempt to accumulate tof difference samples based on the last measurement
 * until we have accumulated the desired number of samples.  
 * Return 1 if we have reached the desired number and 0 otherwise.
 */

uint8_t accumulate_samples()
{
	uint8_t i, j;

	for (i=0; i<kNumTOFPulses; ++i)
	{
		int32_t val = INT32_MAX;
		int32_t dist;

		if (tof1[i] == 0)
			continue;

		for (j=0; j<kNumTOFPulses; ++j)
		{
			if (tof2[j] == 0)
				continue;
			
			dist = tof1[i] - tof2[j];

			if (abs32(dist) > INT16_MAX)
				continue;

			val = dist;
			break;
		}

		if (val == INT32_MAX)
			continue;

		delta_tof_accum += val;
		num_measurements += 1;
		
		if (num_measurements == desired_measurements)
			return 1;	
	}

	return 0;
}

int32_t abs32(int32_t val)
{
	if (val < 0)
		return -val;

	return val;
}

UltrasoundError wait_for_measurement(uint8_t timeout)
{
	uint8_t result;
	tdc1000_error 		err1000;
	tdc7200_intstatus	err7200;

	tmr_config(MEASUREMENT_TIMER, kTMRPrescale_64, kTMRPostscale1_16);
	tmr_intenable(MEASUREMENT_TIMER) = 0;
	tmr_flag(MEASUREMENT_TIMER) = 0;
	tmr_loadperiod(MEASUREMENT_TIMER, timeout);
	tmr_load(MEASUREMENT_TIMER, 0);
	tmr_setstate(MEASUREMENT_TIMER, 1);

	while (PIN(INT7200) && PIN(ERR1000))
	{
		if (tmr_flag(MEASUREMENT_TIMER))
			break;
	}

	err1000.value = tdc1000_readfast(kTDC1000_ErrorFlagsReg);
	err7200.value = tdc7200_readfast(kTDC7200_INTStatusReg);

	if (err1000.no_sig)
		return kNoSignalError;
	else if(err1000.sig_weak)
		return kWeakSignalError;

	tmr_setstate(MEASUREMENT_TIMER, 0);

	if (tmr_flag(MEASUREMENT_TIMER))
		return kTimeoutError;

	return kNoUltrasoundError;
}

uint8_t take_measurement()
{
	uint8_t retval;

	tdc7200_trigger();
	retval = wait_for_measurement(255);

	return retval;
}

int16_t calculate_mean()
{
	int32_t mean = 0;
	uint8_t i;

	for (i=0; i<kMeasurementBufferSize; ++i)
		mean += measurements[i];
	
	return (mean >> 7);
}

int32_t calculate_variance()
{
	int16_t mean = calculate_mean();
	int32_t accum = 0;
	uint8_t i;

	for (i=0; i<kMeasurementBufferSize; ++i)
	{
		int32_t diff = measurements[i] - mean;
		accum += diff*diff;
	}

	return (accum >> 7);
}
