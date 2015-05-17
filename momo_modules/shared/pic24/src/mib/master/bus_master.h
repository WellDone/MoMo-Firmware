#ifndef __bus_master_h__
#define __bus_master_h__

#include "bus.h"
#include "rtcc.h"

#define kMaxAsyncRPCNUmber	16

typedef struct
{
	uint8_t			 	sender;
	uint8_t				flags;
	mib_rpc_function 	callback;
} async_rpc_data;

//Master Routines
void bus_master_callback();
void bus_master_rpc_async(mib_rpc_function callback, MIBUnified *data);
void bus_master_init();
void bus_master_finish_async_rpc(uint8_t sender);

unsigned int bus_master_idle();
rtcc_timestamp bus_master_rpc_start_timestamp();
#endif
