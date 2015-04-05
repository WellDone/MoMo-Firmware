#ifndef __bus_master_h__
#define __bus_master_h__

#include "bus.h"
#include "rtcc.h"

//Master Routines
void bus_master_callback();
void bus_master_rpc_async(mib_rpc_function callback, MIBUnified *data);
void bus_master_init();
unsigned int bus_master_idle();
rtcc_timestamp bus_master_rpc_start_timestamp();
#endif
