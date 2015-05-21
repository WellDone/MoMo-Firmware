//mib_hal.h 
/*
 * Most of the MIB code is portable between pic24 and pic12 but some functions are 
 * needed that benefit from optimization on both platforms separately.  All the non-common
 * functions are declared in this header.  Each platform implementing the MIB must
 * include a conforming implementation of these functions.
 */

#ifndef __mib_hal_h__
#define __mib_hal_h__

#include "common_types.h"
#include <stdint.h>

#define kInvalidMIBIndex 255

uint8_t call_handler(uint8 handler_index);
uint8_t find_handler(void);
void  	bus_init(uint8 address);
void 	bus_slave_seterror(uint8 error);
uint8_t bus_is_idle();

#endif