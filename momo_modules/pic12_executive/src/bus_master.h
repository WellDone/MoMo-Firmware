#ifndef __bus_master_h__
#define __bus_master_h__

#include "bus.h"

//Master Routines
#define			bus_master_prepare(ftr, cmd, spc)  			\
				{											\
					mib_data.bus_command.feature = ftr;	\
					mib_data.bus_command.command = cmd;	\
					mib_data.bus_command.param_spec = spc;	\
				}

uint8			bus_master_rpc_sync(unsigned char address);

#endif
