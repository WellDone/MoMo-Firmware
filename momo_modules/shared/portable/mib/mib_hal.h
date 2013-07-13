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

void  call_handler(uint8 handler_index);
uint8 validate_params(uint8 handler_index);
uint8 find_handler(void);
uint8 loadparams(uint8 param_spec);
void  bus_init();

#endif