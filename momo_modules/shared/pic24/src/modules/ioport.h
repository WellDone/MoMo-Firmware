#ifndef __ioport_h__
#define __ioport_h__

//Macros for controlling IO ports
#define DIR_R(x) 		(_TRIS##x)
#define LAT_R(x)		(_LAT##x)
#define PIN_R(x)		(_R##x)
#define TYPE_R(x)		(_ANS##x)

#define DIR(x)			DIR_R(x)
#define LAT(x)			LAT_R(x)
#define PIN(x)			PIN_R(x)
#define TYPE(x)			TYPE_R(x)

#define INPUT			1
#define OUTPUT			0

#define ANALOG 			1
#define DIGITAL			0

#ifdef __PIC24FJ64GA306__
//Define all support peripheral input names
#define SDI1_INPUT 		_SDI1R

//Define all supported peripheral output names
enum
{
	SDO1_OUTPUT	= 7,
	SCK1_OUTPUT = 8
};

#define MAP_PERIPHERAL_OUT_R(pin, output)	_RP##pin##R = (output)

#define MAP_PERIPHERAL_OUT(pin, output)		MAP_PERIPHERAL_OUT_R(pin, output)
#define MAP_PERIPHERAL_IN(pin, input)		input = (pin)
#endif

#endif