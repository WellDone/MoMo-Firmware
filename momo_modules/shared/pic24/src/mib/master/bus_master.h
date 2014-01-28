#ifndef __bus_master_h__
#define __bus_master_h__

#include "bus.h"

//Master Routines
void 			bus_master_callback();
#ifndef _PIC12
void 			bus_master_rpc_async(mib_rpc_function callback, unsigned char address, unsigned char feature, unsigned char cmd, unsigned char spec);
void      bus_master_init();
#else
#define			bus_master_prepare(ftr, cmd, spc)  			\
				{											\
					mib_state.bus_command.feature = ftr;	\
					mib_state.bus_command.command = cmd;	\
					mib_state.bus_command.param_spec = spc;	\
				}

uint8			bus_master_rpc_sync(unsigned char address);

#endif

#endif
