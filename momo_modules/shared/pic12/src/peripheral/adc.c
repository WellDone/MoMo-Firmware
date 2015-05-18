//adc.c

#define __ADC_INTERNAL__
#include "adc.h"
#include <stdint.h>

unsigned int 	adc_result;

void adc_setenabled(uint8_t on)
{
	ADON = on;
}

void adc_convertsync()
{
	ADIF = 0;
	GO = 1;

	while(!ADIF)
		;
}

void adc_setchannel(uint8_t chan)
{
	ADCON0 &= (~kADCChannelMask);
	ADCON0 |= chan;
}

void adc_configure(uint8_t config)
{
	ADCON1 = config;
}

void adc_average(uint8_t n)
{
	uint8_t i;

	adc_result = 0;

	for(i=0; i<n; ++i)
	{
		adc_convertsync();
		
		adc_result += ADRESL | (ADRESH << 8);
	}

	adc_result /= n;
}