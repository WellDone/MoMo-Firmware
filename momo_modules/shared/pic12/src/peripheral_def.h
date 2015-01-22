#ifndef __peripheral_def_h__
#define __peripheral_def_h__

#include "bit_utilities.h"

/*
 * Macros to support defining perihperal functions that can be reused
 * with multiple peripherals of the same type.  The PIC12 has family
 * standard peripherals so it makes sense to write just 1 version of the
 * code for each peripheral and use macros to choose the right registers
 * to control, e.g. mssp1 vs mssp2 since the two are identical, just in 
 * different bank locations.
 */

#define pdefine_f(periph, num, fun)	periph##num##_##fun
#define pdefine_r(periph, num, fun)	periph##num##fun
#define p_fun(p, n, f)				pdefine_f(p, n, f)
#define p_reg(p, n, f)				pdefine_r(p, n, f)

 /*
  * Common Bit Utilities
  */
#define bit_set(x)		SET_BIT(x)
#define bit_clear(x)	CLEAR_BIT(x)
#define bit(x)			BIT(x)			

 /*
  * Common Peripheral-Wide Definitions
  *
  */

 typedef enum
 {
 	kDisabled = 0,
 	kEnabled_Sync = 1,
 	kEnabled_Async = 2
 } PeripheralState;

#endif