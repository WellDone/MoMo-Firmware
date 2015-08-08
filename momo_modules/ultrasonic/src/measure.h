#ifndef __measure_h__
#define __measure_h__

#include <stdint.h>

#define kNumTOFPulses				5
#define kNumberOfTriggerSettings	128

//Do not change without also changing bit shift amounts in routines like calculate_mean
#define kMeasurementBufferSize		128

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
void 			set_gain(uint8_t gain, uint8_t threshold);
void 			set_gain_index(uint8_t index);


UltrasoundError measure_delta_tof(uint8_t control_power, uint8_t averages);

uint8_t 		take_measurement();
UltrasoundError wait_for_measurement(uint8_t timeout);

void 			fill_parameters();

UltrasoundError accumulate_delta_tof(uint8_t bits);
UltrasoundError fast_accumulate_delta_tof(uint8_t bits);
uint8_t 		accumulate_samples();

int32_t 		abs32(int32_t val);

//Measurement Buffer routines
int16_t 		calculate_mean();
int32_t 		calculate_variance();

#endif
