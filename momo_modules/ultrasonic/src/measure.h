#ifndef __measure_h__
#define __measure_h__

#include <stdint.h>

typedef struct
{
	uint8_t gain;
	uint8_t threshold;
} trigger_condition;

typedef enum 
{
	kNoUltrasoundError = 0,
	kNoSignalError,
	kWeakSignalError,
	kTimeoutError
} UltrasoundError;

void 			initialize_parameters();
void 			set_parameters(uint8_t gain, uint8_t threshold, uint8_t pulses, uint16_t mask);

UltrasoundError measure_delta_tof(uint8_t control_power, uint8_t averages);

uint8_t 		take_measurement();
UltrasoundError wait_for_measurement(uint8_t timeout);

void 			sort_measurements();

void 			fill_parameters();

uint8_t 		noise_floor_index();
uint32_t 		noise_floor_voltage(uint32_t *threshold, uint32_t *gain);
int32_t 		find_variance(const trigger_condition cond);
UltrasoundError optimize_settings(trigger_condition *out, int32_t *out_variance);
UltrasoundError accumulate_delta_tof(uint8_t bits);
uint8_t 		accumulate_samples();

int32_t abs32(int32_t val);

#endif
