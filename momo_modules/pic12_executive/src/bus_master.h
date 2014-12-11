#ifndef __bus_master_h__
#define __bus_master_h__

#include "bus.h"

//Master Routines

void			bus_master_begin_rpc();
#define			bus_master_prepare(ftr, cmd, spc)  			\
				{											\
					mib_data.bus_command.feature = ftr;	\
					mib_data.bus_command.command = cmd;	\
					mib_data.bus_command.param_spec = spc;	\
				}

uint8			bus_master_send_rpc(unsigned char address);
uint8			bus_master_send(uint8 length);
uint8			bus_master_receive(uint8 length);

uint8			get_mib_result(); //Return the result portion of the mib return header		
#endif
