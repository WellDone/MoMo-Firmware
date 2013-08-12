//mib12_api.h

#ifndef __mib12_api_h__
#define __mib12_api_h_

#include "protocol.h"

uint8 bus_master_rpc_sync(uint8 address);
void  bus_slave_setreturn(uint8 status);

#define mib_buffer_length()			mib_packet.param_spec & 0b00011111

extern bank1 unsigned char 			mib_buffer[kBusMaxMessageSize];
extern bank1 MIBCommandPacket 		mib_packet;

#endif