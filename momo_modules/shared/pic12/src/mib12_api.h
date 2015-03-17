//mib12_api.h

#ifndef __mib12_api_h__
#define __mib12_api_h_

#include "protocol.h"

void  trap(uint8 code);
void  reset_device();
void  bus_master_begin_rpc();
uint8 bus_master_send_rpc(uint8 address);
void  bus_slave_setreturn(uint8 status);

#define mib_buffer_length()			mib_packet.param_spec & 0b00011111

extern bank1 unsigned char 		mib_buffer[kBusMaxMessageSize];
extern bank1 MIBCommandPacket   mib_packet;
extern bank1 uint8              slave_address;

#define mib_address             (slave_address >> 1)
#define plist_get_int8(n)				mib_buffer[(n<<1) ]
#define plist_get_int16(n)			((int*)mib_buffer)[n]

#define bus_master_prepare_rpc(f, c, s) { \
	mib_packet.feature = f; \
	mib_packet.command = c; \
	mib_packet.param_spec = s;}

#endif