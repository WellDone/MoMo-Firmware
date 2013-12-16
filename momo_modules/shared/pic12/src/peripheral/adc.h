//adc.h

#ifndef __adc_h__
#define __adc_h__

#include <xc.h>

#define kADC_CHAN_MASK			0b11111
#define BUILD_ADC_CHAN(x)		((x & kADC_CHAN_MASK) << 2)

//ADC Channel Constants
#define kADCChannelAN0			BUILD_ADC_CHAN(0)
#define kADCChannelAN1			BUILD_ADC_CHAN(1)
#define kADCChannelAN2			BUILD_ADC_CHAN(2)
#define kADCChannelAN3			BUILD_ADC_CHAN(3)
#define kADCChannelAN4			BUILD_ADC_CHAN(4)
#define kADCChannelAN5			BUILD_ADC_CHAN(5)
#define kADCChannelAN6			BUILD_ADC_CHAN(6)
#define kADCChannelAN7			BUILD_ADC_CHAN(7)
#define kADCChannelAN8			BUILD_ADC_CHAN(8)
#define kADCChannelAN9			BUILD_ADC_CHAN(9)
#define kADCChannelAN10			BUILD_ADC_CHAN(10)
#define kADCChannelAN11			BUILD_ADC_CHAN(11)
#define kADCChannelTemp			BUILD_ADC_CHAN(0b11101)
#define kADCChannelDAC			BUILD_ADC_CHAN(0b11110)
#define kADCChannelFVR			BUILD_ADC_CHAN(0b11111)

//ADC Clock Constants
#define kADC_CLOCK_MASK			0b111
#define BUILD_ADC_CLOCK(x)		((x & kADC_CLOCK_MASK) << 4)

//ADC Conversion Clockrates
#define kFRC					BUILD_ADC_CLOCK(0b111)

//ADC Justification Modes (right justified is shifted all to low order bits)
#define kADCRightJustified 		(1 << 7)
#define kADCLeftJustified		0

//ADC Voltage References
#define BUILD_ADC_POSREF(x)		((x & 0b11))
#define BUILD_ADC_NEGREF(x)		((x & 0b1) << 1)

//Positive References
#define kADCVDDPosRef			BUILD_ADC_POSREF(0)
#define kADCExtPosRef			BUILD_ADC_POSREF(0b10)
#define kADCFVRPosRef			BUILD_ADC_POSREF(0b11)

//Negative References
#define kADCVSSNegRef			BUILD_ADC_NEGREF(0)
#define kADCExtNegRef			BUILD_ADC_NEGREF(1)

#endif