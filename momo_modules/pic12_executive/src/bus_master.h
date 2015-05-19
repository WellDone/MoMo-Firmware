#ifndef __bus_master_h__
#define __bus_master_h__

#include "bus.h"

//Master Routines

void	bus_master_begin_rpc();
uint8	bus_master_send_rpc(unsigned char address);

#endif
