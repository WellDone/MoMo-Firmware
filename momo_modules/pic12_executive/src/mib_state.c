//mib_state.c

/*
 * Define all the shared mib global variables in one spot so that we can give them appropriate locations in memory
 */

#define __NO_EXTERN_MIB_STATE__
#include "bus.h"
#include "mib_state.h"
#include "i2c.h"
#include "bootloader.h"

bank1 MIBData					      mib_data;						//Put mib data in first byte of GPR in bank1 (24 bytes)
bank1 MIBState 						  mib_state;					//Put mib state right after mib_buffer (5 bytes)
bank1 __persistent MIBExecutiveStatus status;						//MIBExecutiveStatus (1 byte)

//bank1 is completely full
