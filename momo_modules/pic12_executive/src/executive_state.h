#ifndef __executive_state_h__
#define __executive_state_h__

#include "protocol.h"

//3 bytes long
typedef struct 
{
	//Master variables
	uint8_t  	slave_address;
	uint8_t		send_address;

	//Slave variables
	uint8_t 	curr_loc;
} MIBState;

//1 Byte
//These bits are referenced in c and assembly code so they cannot change position without
//also updating asm_locations.h
typedef union
{
	struct
	{
		uint8_t respond_busy 		: 1;
		uint8_t valid_app 		: 1;
		uint8_t async_callback 	: 1;
		uint8_t registered		: 1;
		uint8_t dirty_reset 		: 1; 
		uint8_t master_waiting 	: 1;
		uint8_t first_read		: 1;
		uint8_t trapped			: 1;
	};

	uint8_t status;
} MIBExecutiveStatus;

extern MIBPacket						mib_packet;
extern MIBState 						mib_state;
extern __persistent MIBExecutiveStatus	status;

#endif
