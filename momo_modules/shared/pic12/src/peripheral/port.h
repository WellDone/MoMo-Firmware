//port.h

#ifndef __port_h__
#define __port_h__

//Internal macros, not for use
#define LATCHR(pin)		LAT##pin
#define TRISR(pin)		TRIS##pin
#define TYPER(pin)		ANS##pin
#define PORTR(pin)		R##pin

//Macros for end-user use
#define OUTPUT			0
#define INPUT			1

#define DIGITAL 		0
#define ANALOG 			1

#define LATCH(pin)		LATCHR(pin)
#define TRIS(pin)		TRISR(pin)
#define TYPE(pin)		TYPER(pin)
#define PORT(pin)		PORTR(pin)

#define PIN_DIR(pin, dir)	TRIS(pin) = dir
#define PIN_SET(pin, val)	LATCH(pin) = val
#define PIN_TYPE(pin, type) TYPE(pin) = type
#define PIN(pin) 			(PORT(pin))

#endif