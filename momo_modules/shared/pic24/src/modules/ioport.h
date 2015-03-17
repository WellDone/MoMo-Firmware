#ifndef __ioport_h__
#define __ioport_h__

//Macros for controlling IO ports
#define DIR_R(x) 			(_TRIS##x)
#define LAT_R(x)			(_LAT##x)
#define PIN_R(x)			(_R##x)
#define TYPE_R(x)			(_ANS##x)
#define ENSURE_DIGITAL_R(x)	ENSURE_DIGITAL_##x

#define DIR(x)				DIR_R(x)
#define LAT(x)				LAT_R(x)
#define PIN(x)				PIN_R(x)
#define TYPE(x)				TYPE_R(x)
#define ENSURE_DIGITAL(x)	ENSURE_DIGITAL_R(x)

#define INPUT			1
#define OUTPUT			0

#define ANALOG 			1
#define DIGITAL			0

#ifdef __PIC24FJ64GA306__

//Define all support peripheral input names
#define SDI1_INPUT 		_SDI1R
#define SCK1_INPUT		_SCK1R
#define UART1_RX		_U1RXR
#define UART1_CTS 		_U1CTSR

//Define all supported peripheral output names
enum
{
	SDO1_OUTPUT	= 7,
	SCK1_OUTPUT = 8,

	UART1_RTS = 4,
	UART1_TX = 3
};

#define MAP_PERIPHERAL_OUT_R(pin, output)	_RP##pin##R = (output)

#define MAP_PERIPHERAL_OUT(pin, output)		MAP_PERIPHERAL_OUT_R(pin, output)
#define MAP_PERIPHERAL_IN(pin, input)		input = (pin)

//Record the pins that could be analog on PIC24FJ64GA306 so that we 
//can make them digital without having to hardcode which ones are
//analogable and not
#define ENSURE_DIGITAL_B0 					_ANSB0 = 0
#define ENSURE_DIGITAL_B1 					_ANSB1 = 0
#define ENSURE_DIGITAL_B2 					_ANSB2 = 0
#define ENSURE_DIGITAL_B3 					_ANSB3 = 0
#define ENSURE_DIGITAL_B4 					_ANSB4 = 0
#define ENSURE_DIGITAL_B5 					_ANSB5 = 0
#define ENSURE_DIGITAL_B6 					_ANSB6 = 0
#define ENSURE_DIGITAL_B7 					_ANSB7 = 0
#define ENSURE_DIGITAL_B8 					_ANSB8 = 0
#define ENSURE_DIGITAL_B9 					_ANSB9 = 0
#define ENSURE_DIGITAL_B10 					_ANSB10 = 0
#define ENSURE_DIGITAL_B11					_ANSB11 = 0
#define ENSURE_DIGITAL_B12					_ANSB12 = 0
#define ENSURE_DIGITAL_B13 					_ANSB13 = 0
#define ENSURE_DIGITAL_B14					_ANSB14 = 0
#define ENSURE_DIGITAL_B15					_ANSB15 = 0

#define ENSURE_DIGITAL_F0
#define ENSURE_DIGITAL_F1
#define ENSURE_DIGITAL_F2
#define ENSURE_DIGITAL_F3
#define ENSURE_DIGITAL_F4
#define ENSURE_DIGITAL_F5
#define ENSURE_DIGITAL_F6
#define ENSURE_DIGITAL_F7

#define ENSURE_DIGITAL_G0
#define ENSURE_DIGITAL_G1
#define ENSURE_DIGITAL_G2
#define ENSURE_DIGITAL_G3
#define ENSURE_DIGITAL_G4
#define ENSURE_DIGITAL_G5
#define ENSURE_DIGITAL_G6
#define ENSURE_DIGITAL_G7

#define ENSURE_DIGITAL_D0
#define ENSURE_DIGITAL_D1
#define ENSURE_DIGITAL_D2
#define ENSURE_DIGITAL_D3
#define ENSURE_DIGITAL_D4
#define ENSURE_DIGITAL_D5
#define ENSURE_DIGITAL_D6
#define ENSURE_DIGITAL_D7
#define ENSURE_DIGITAL_D8
#define ENSURE_DIGITAL_D9
#define ENSURE_DIGITAL_D10
#define ENSURE_DIGITAL_D11
#define ENSURE_DIGITAL_D12
#define ENSURE_DIGITAL_D13
#define ENSURE_DIGITAL_D14
#define ENSURE_DIGITAL_D15
#endif

#endif