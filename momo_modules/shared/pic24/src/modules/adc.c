#include "adc.h"
#include <pic24.h>

task_callback adc_callback = 0;
volatile unsigned int oneshot = 0;

volatile unsigned int adc_buffer[kADCBufferSize];

void adc_configure(const ADCConfig *config)
{
	adc_disable();

	_ADSIDL = config->enable_in_idle;
	_FORM 	= config->output_format;
	_SSRC 	= config->trigger;
	_ASAM 	= config->sample_autostart;
	_VCFG 	= config->reference;
	_CSCNA 	= config->scan_input;
	_SMPI 	= kADCInterruptRate;
	_BUFM 	= 1; //Enable double buffering with 2 8 sample buffers
	_ALTS	= 0; //Only sample MUX A (don't use MUX B ever)
	_ADRC 	= 1; //Use the internal clock (b/c the oscillator speed of the PIC can change)
	_SAMC	= config->autosample_wait;

	//Setup interupt
	_AD1IF = 0; //Clear interrupt flag
	//_AD1IE = 1; //Enable interrupt
	//_AD1IP = 1; //Low priority interrupt

	if (config->oneshot)
	{
		_SMPI = config->num_samples - 1;
		oneshot = config->oneshot;
	}
}

void adc_set_status(unsigned int enabled)
{
	_ADON = enabled;
}

unsigned int adc_convert_one()
{
    unsigned int value;

    _AD1IE = 0; //disable interrupt
    _SMPI = 0; //interrupt after 1 conversion (we're checking the flag manually)
    _AD1IF = 0;

    adc_enable();

    while (!_AD1IF)
        ;
    adc_disable();

    value = ADC1BUF0;

    //Set everything back to the way it was (this needs to stay synced with adc_configure
    _AD1IF = 0;
    //_AD1IE = 1;
    _SMPI = kADCInterruptRate;

    return value;
}

void adc_setup_scan(unsigned int channels)
{
	AD1CSSL = channels;

	//Set corresponding ports to analog and enable bg reference if any
	AD1PCFG &= ~channels;

	_CSCNA = 1; //Enable scanning
}

void adc_set_channel(unsigned int channel)
{
	_CSCNA = 0; //disable scanning
	_CH0SA = channel;
}

void set_callback(task_callback handler)
{
	adc_callback = handler;
}

void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt()
{
	if (_BUFS)
	{
		adc_buffer[0] = ADC1BUF0;
		adc_buffer[1] = ADC1BUF1;
		adc_buffer[2] = ADC1BUF2;
		adc_buffer[3] = ADC1BUF3;
		adc_buffer[4] = ADC1BUF4;
		adc_buffer[5] = ADC1BUF5;
		adc_buffer[6] = ADC1BUF6;
		adc_buffer[7] = ADC1BUF7;
	}
	else
	{
		adc_buffer[0] = ADC1BUF8;
		adc_buffer[1] = ADC1BUF9;
		adc_buffer[2] = ADC1BUFA;
		adc_buffer[3] = ADC1BUFB;
		adc_buffer[4] = ADC1BUFC;
		adc_buffer[5] = ADC1BUFD;
		adc_buffer[6] = ADC1BUFE;
		adc_buffer[7] = ADC1BUFF;
	}

	if (oneshot)
		adc_disable();

	if (adc_callback)
		taskloop_add(adc_callback);

	_AD1IF = 0;
}