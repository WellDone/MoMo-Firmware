//bus_state.h

/*
 * Make sure files have access to the fundamental MIB global state variables so that
 * we can replace functions with macros that reference these variables.
 */

#ifndef __NO_EXTERN_MIB_STATE__
#include "protocol.h"

extern MIBState 			mib_state;
extern bank1 unsigned char 	mib_buffer[kBusMaxMessageSize];
extern bank1 unsigned char 	mib_firstfree;

#endif