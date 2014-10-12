//port.h

#ifndef __port_h__
#define __port_h__

//Internal macros, not for use (needed to properly expand macros in arguments)
#define LATCHR(pin)					LAT##pin
#define TRISR(pin)					TRIS##pin
#define TYPER(pin)					ANS##pin
#define PORTR(pin)					R##pin
#define ENSURE_DIGITALR(pin)		ENSURE_DIGITAL_##pin
#define ANALOG_IF_POSSIBLER(pin)	TRY_ANALOG_##pin

/*
 * Macros for end-user use
 */

//Valid Pin Directions
#define OUTPUT			0
#define INPUT			1

//Valid Pin Types
#define DIGITAL 		0
#define ANALOG 			1

//Defined pin actions
#define LATCH(pin)		LATCHR(pin)
#define TRIS(pin)		TRISR(pin)
#define TYPE(pin)		TYPER(pin)
#define PORT(pin)		PORTR(pin)

#define PIN_DIR(pin, dir)	TRIS(pin) = dir
#define PIN_SET(pin, val)	LATCH(pin) = val
#define PIN_TYPE(pin, type) TYPE(pin) = type
#define PIN(pin) 			(PORT(pin))

#include "port_definitions.h"

#define ENSURE_DIGITAL(pin)		ENSURE_DIGITALR(pin)
#define ANALOG_IF_POSSIBLE(pin) ANALOG_IF_POSSIBLER(pin)

#endif