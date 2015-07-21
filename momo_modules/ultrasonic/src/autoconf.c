#include "autoconf.h"
#include "communication.h"
#include "tdc1000.h"
#include "tdc7200.h"

extern int32_t 	tof1[kNumTOFPulses];
extern int32_t 	tof2[kNumTOFPulses];
extern int16_t 	measurements[128];

extern const trigger_condition triggers[kNumberOfTriggerSettings];

uint8_t find_signal_strength()
{
	int8_t i;

	tdc7200_setaverages(0);
	enable_power();

	for (i=kNumberOfTriggerSettings-1; i >= 0; --i)
	{
		UltrasoundError err;
		trigger_condition trigger;

		trigger = triggers[i];

		set_gain(trigger.gain, trigger.threshold);
		tdc1000_pushfast();

		err = measure_delta_tof(0, 0);
		if (err != kNoUltrasoundError)
			continue;

		//Check if we've found a signal
		if (tof1[0] > 0 && tof2[0] > 0)
		{
			disable_power();
			return i;
		}
	}

	disable_power();
	return 255;
}

UltrasoundError find_optimal_gain(int32_t *out_variance, uint8_t *out_i)
{
	uint8_t i;
	int32_t variance;
	int32_t delta;
	int32_t best_variance = INT32_MAX;
	uint8_t best_i = 255;

	uint8_t start = find_signal_strength();
	
	if (start == 255)
		return kNoSignalError;

	tdc7200_setaverages(0);
	enable_power();

	for (i=0; i<=start; ++i)
	{
		uint8_t j = 0;

		UltrasoundError err;
		trigger_condition trigger;

		trigger = triggers[i];

		set_gain(trigger.gain, trigger.threshold);
		tdc1000_prepare_deltatof(7);
		tdc1000_pushfast();
		for (j=0; j<128; ++j)
		{
			err = measure_delta_tof(0, 0);

			if (err != kNoUltrasoundError)
				break;

			delta = tof1[0] - tof2[0];
			if (delta < INT16_MIN || delta > INT16_MAX)
				break;

			measurements[j] = delta;
		}

		if (j == 128)
		{
			variance = calculate_variance();
			if (variance < best_variance)
			{
				best_i = i;
				best_variance = variance;
			}
		}
	}

	disable_power();

	if (best_i == 255)
		return kWeakSignalError;

	*out_variance = best_variance;
	*out_i = best_i;

	return kNoUltrasoundError;
}

UltrasoundError find_pulse_variance(int32_t *out_variances)
{
	uint8_t i, j;
	int32_t variance;
	UltrasoundError err;

	tdc7200_setaverages(0);
	tdc1000_prepare_deltatof(7);

	enable_power();

	for (j=0; j<5; ++j)
	{
		tdc1000_pushfast();
		for (i=0; i<128; ++i)
		{
			int32_t delta;

			err = measure_delta_tof(0, 0);

			if (err != kNoUltrasoundError)
				break;

			delta = tof1[j] - tof2[j];
			if (delta < INT16_MIN || delta > INT16_MAX)
				break;

			measurements[i] = delta;
		}

		if (i == 128)
		{
			variance = calculate_variance();
			out_variances[j] = variance;
		}
		else
			out_variances[j] = 0;
	}

	disable_power();

	return kNoUltrasoundError;
}