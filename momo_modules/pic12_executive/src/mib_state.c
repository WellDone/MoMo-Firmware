//mib_state.c

/*
 * Define all the shared mib global variables in one spot so that we can give them appropriate locations in memory
 */

#define __NO_EXTERN_MIB_STATE__
#include "bus.h"
#include "i2c.h"

bank1 unsigned char 	mib_buffer[kBusMaxMessageSize];		//Put mib state in first byte of GPR in bank1 (20 bytes)
bank1 MIBState 		mib_state;							//Put mib state right after mib_buffer (10 bytes)
bank1 volatile I2CStatus i2c_status;						//Put i2c_status new, (2 bytes)

//0 bytes left in bank 1