#ifndef __measure_h__
#define __measure_h__

#include <stdint.h>

typedef struct
{
	uint8_t gain;
	uint8_t threshold;
} trigger_condition;

enum UltrasoundError
{
	kNOUltrasoundError,
	kNoSignalError,
	kWeakSignalError,
	kHighNoiseError
};

void 	set_parameters(uint8_t gain, uint8_t threshold, uint8_t pulses, uint16_t mask);
UltrasoundError measure_delta_tof(int32_t *out);

uint8_t take_measurement();
void 	sort_measurements();

#endif
