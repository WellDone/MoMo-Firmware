//mib_state.c

/*
 * Define all the shared mib global variables in one spot so that we can give them appropriate locations in memory
 * mib_state and mib_data are defined in assembly in mib_state_asm.as so that they can have absolute addresses that
 * are guaranteed not to change based on xc8's whim that day.
 */

#define __NO_EXTERN_MIB_STATE__
#include "bus.h"
#include "mib_state.h"
#include "i2c.h"
#include "bootloader.h"

bank1 __persistent MIBExecutiveStatus status;						//MIBExecutiveStatus (1 byte)
