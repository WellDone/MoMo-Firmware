#ifndef __platform_h__
#define __platform_h__

#include <pic12f1822.h>
#include "common_types.h"
#include "bit_utilities.h"

#define kMIBEndpointAddress		0x7FA		//the last 6 bytes of program memory
#define kMIBMagicNumber			0xAA

#pragma switch space

/*
 * The pic12 skeleton code looks for a MIBEndpointInfo structure
 * at a specific address at the top of program memory and uses this 
 * to find handlers.  If there is no application code loaded, the magic
 * number will be wrong and MIB will not load any application endpoints.
 */

//Do not compile components that are not needed on the pic12 to make sure the compiler doesn't choke
#define _PIC12
#define _PIC12LEAN
#define _MACRO_SMALL_FUNCTIONS

//PIC12 does not have separate master and slave statuses
#define i2c_slave_lasterror()	i2c_lasterror()
#define i2c_master_lasterror()	i2c_lasterror()
#define i2c_slave_state()		i2c_state()
#define i2c_master_state()		i2c_state()

uint8 get_magic();

#endif