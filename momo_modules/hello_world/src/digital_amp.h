#ifndef __digital_amp_h__
#define __digital_amp_h__

#include "port.h"
#include <stdint.h>

#include <xc.h>

typedef enum
{
	kOffsetParameter = 0,
	kStage1GainParameter = 1,
	kStage2GainParameter = 2,
	kSelectCurrentParameter = 3,
	kInvertInputParamter = 4,
	kStabilizationDelayMS = 5,
	kNumParameters = 6
} AmplifierSetting;

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


void 	damp_set_parameter(AmplifierSetting setting, uint8_t value);
uint8_t damp_get_parameter(AmplifierSetting setting);

void damp_init();
void damp_enable();
void damp_disable();

#endif