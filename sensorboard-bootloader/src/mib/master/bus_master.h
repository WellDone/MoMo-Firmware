#ifndef __bus_master_h__
#define __bus_master_h__

#include "bus.h"

//Master Routines
void 			bus_master_callback();
int 			bus_master_rpc(mib_rpc_function callback, unsigned char address, unsigned char feature, unsigned char cmd, MIBParameterHeader **params, unsigned char param_count);
#endif
