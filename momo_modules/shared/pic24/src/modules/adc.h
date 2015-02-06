#ifndef __adc_h__
#define __adc_h__

#include "task_manager.h"
#include "bit_utilities.h"

//Convenience Macros
#define adc_disable() 				adc_set_status(0)
#define adc_enable()				adc_set_status(1)
#define ADD_CHANNEL(x, channel)	SET_BIT(x, channel)

//Hardcoded Constants
#define kADCInterruptRate		7		//Interrupt every N+1 samples are aquired (basically acquire a full 8 sample buffer)
#define kADCBufferSize			8

//Support for different PIC24 processors
#ifdef __PIC24FJ64GA306__
#define ADC1BUFA		ADC1BUF10
#define ADC1BUFB		ADC1BUF11
#define ADC1BUFC		ADC1BUF12
#define ADC1BUFD		ADC1BUF13
#define ADC1BUFE		ADC1BUF14
#define ADC1BUFF		ADC1BUF15
#endif

//Additinal Channel Scan selection bits
enum
{
	kVSSReference = 0b110,
	kVDDReference = 0b111,
	kNoChannelsConnected = 0b1101,
	kHalfBandgapReference = 14,
	kBandgapReference = 15,
};


/*
 * Configuration shortcuts
 */

#define kADCOutputFormatMask 	0b11
typedef enum
{
	kUIntegerFormat = 0b00,
	kSIntegerFormat = 0b01,
	kUFractionalFormat = 0b10,
	kSFractionalFormat = 0b11
} ADCOutputFormat;

#define kADCTriggerSourceMask 	0b111
typedef enum
{
	kProgramControl = 0b000,
	kINTOTransition = 0b001,
	kTimer3Match = 0b010,
	kCTMUEvent = 0b110,
	kInternalCounter = 0b111
} ADCTriggerSource;

#define kADCVoltageReferenceMask 0b11
typedef enum
{
	kVDDVSS = 0b000
} ADCVoltageReference;

typedef struct
{
	ADCOutputFormat 	output_format;
	ADCTriggerSource 	trigger;
	ADCVoltageReference reference;

	unsigned int enable_in_idle : 1;
	unsigned int sample_autostart : 1;
	unsigned int scan_input : 1;
	//unsigned int double_buffer : 1; //NOT IMPLEMENTED
	unsigned int alternate_muxes : 1;
	//unsigned int use_internal_clock : 1; //NOT IMPLEMENTED

	//sampling delays
	unsigned int autosample_wait: 5;
	//unsigned int conversion_period: 6; //NOT IMPLEMENTED (always using ADC internal RC clock)

	//Configure one-shot sampling
	unsigned int oneshot: 1; //When module is enabled, only collect one set of samples
	unsigned int num_samples; //Number of samples to collect (must be 1 <= X <= 8)
} ADCConfig;

//Module Control Functions
void adc_configure(const ADCConfig *config);

void adc_setup_scan(unsigned int channels);
void adc_set_channel(unsigned int channel);
unsigned int adc_convert_one();

void adc_set_status(unsigned int enabled);

void set_callback(task_callback handler);

#endif