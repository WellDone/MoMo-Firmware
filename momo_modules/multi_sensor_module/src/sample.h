//sample.h

#ifndef __sample_h__
#define __sample_h__

typedef enum
{
	kDifferentialInput = 0,
	kCurrentInput = 1
} AnalogInputSelector;

void sample_v1();
void sample_v2();
void sample_v3();
void sample_analog();

void set_analog_power(unsigned char on);
void select_analog_input(AnalogInputSelector sel);

#endif