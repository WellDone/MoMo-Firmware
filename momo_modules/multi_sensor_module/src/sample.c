
#include "sample.h"
#include "mib12_api.h"
#include "adc.h"
#include "port.h"
#include "sensor_defines.h"

#define _XTAL_FREQ			4000000

void sample_v1()
{
	adc_configure(ADCBuildConfig(kADCRightJustified, kFRC, kADCVSSNegRef, kADCVDDPosRef));
	adc_setenabled(1);
	adc_setchannel(kADCChannelAN2);

	adc_average(1);

	adc_setenabled(0);
}

void sample_v2()
{
	adc_configure(ADCBuildConfig(kADCRightJustified, kFRC, kADCVSSNegRef, kADCVDDPosRef));
	adc_setenabled(1);
	adc_setchannel(kADCChannelAN3);

	adc_average(1);

	adc_setenabled(0);
}

void sample_v3()
{
	adc_configure(ADCBuildConfig(kADCRightJustified, kFRC, kADCVSSNegRef, kADCVDDPosRef));
	adc_setenabled(1);
	adc_setchannel(kADCChannelAN9);

	adc_average(1);

	adc_setenabled(0);
}

void sample_analog()
{
	adc_configure(ADCBuildConfig(kADCRightJustified, kFRC, kADCVSSNegRef, kADCVDDPosRef));
	adc_setenabled(1);
	adc_setchannel(kADCChannelAN4);

	adc_average(1);

	adc_setenabled(0);
}

void set_analog_power(unsigned char on)
{
	if (on)
		LATCH(AN_POWER) = 1;
	else
		LATCH(AN_POWER) = 0;

	PIN_DIR(AN_POWER, OUTPUT);
}