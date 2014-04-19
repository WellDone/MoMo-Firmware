#ifndef __digital_amp_h__
#define __digital_amp_h__

#include "sensor_defines.h"
#include "port.h"
#include <stdint.h>

#include <xc.h>

typedef enum
{
	kSimulateParameterValue = 0b01
} AmplifierFunction;

typedef enum
{
	kSecondStageGain = 0b00,
	kFirstStageGain = 0b01,
	kOutputOffset = 0b10,
	kOtherFunction = 0b11
} AmplifierParameter;

void damp_set_offset(uint8_t offset_code);
void damp_set_stage1_gain(uint8_t gain_code);
void damp_set_stage2_gain(uint8_t gain_code);

#endif