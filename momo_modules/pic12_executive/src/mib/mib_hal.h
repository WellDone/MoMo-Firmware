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

void  	bus_init(uint8 address);
uint8 	bus_is_idle();
void	trap(uint8 code);

uint8 exec_reset();

#endif
