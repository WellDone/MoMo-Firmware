#ifndef __platform_h__
#define __platform_h__

#include <xc.h> 
#include "common_types.h"
#include "bit_utilities.h"
#include "constants.h"

#pragma switch space

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
uint8 get_mib_block(uint8 offset);

#endif