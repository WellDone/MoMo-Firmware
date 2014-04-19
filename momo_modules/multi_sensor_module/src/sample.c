
#include "sample.h"
#include "mib12_api.h"
#include "adc.h"

#define _XTAL_FREQ			4000000

void sample_v1()
{
	adc_configure(ADCBuildConfig(kADCRightJustified, kFRC, kADCVSSNegRef, kADCVDDPosRef));
	adc_setenabled(1);
	adc_setchannel(kADCChannelAN4);

	adc_average(10);

	adc_setenabled(0);
}

void sample_v2()
{
	adc_configure(ADCBuildConfig(kADCRightJustified, kFRC, kADCVSSNegRef, kADCVDDPosRef));
	adc_setenabled(1);
	adc_setchannel(kADCChannelAN3);

	adc_average(10);

	adc_setenabled(0);
}

void sample_v3()
{
	adc_configure(ADCBuildConfig(kADCRightJustified, kFRC, kADCVSSNegRef, kADCVDDPosRef));
	adc_setenabled(1);
	adc_setchannel(kADCChannelAN2);

	adc_average(10);

	adc_setenabled(0);
}

void sample_i1()
{
	adc_configure(ADCBuildConfig(kADCRightJustified, kFRC, kADCVSSNegRef, kADCVDDPosRef));
	adc_setenabled(1);
	adc_setchannel(kADCChannelAN9);

	adc_average(10);

	adc_setenabled(0);
}