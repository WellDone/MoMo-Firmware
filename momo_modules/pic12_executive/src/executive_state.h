#ifndef __executive_state_h__
#define __executive_state_h__

#include "protocol.h"

//3 bytes long
typedef struct 
{
	//Master variables
	uint8   slave_address;
	uint8	send_address;

	//Slave variables
	uint8 	curr_loc;
} MIBState;

//1 Byte
typedef union
{
	struct
	{
		uint16 reserved1 		: 1; //cannot change position (referenced by watchdog_asm.as)
		uint16 valid_app 		: 1;
		uint16 bootload_mode 	: 1;
		uint16 registered		: 1;
		uint16 dirty_reset 		: 1; //cannot change, referenced in watchdog_asm.as
		uint16 slave_active 	: 1; //cannot change, referenced in i2c_utilities.as
		uint16 first_read		: 1;
		uint16 trapped			: 1;
	};

	uint16 status;
} MIBExecutiveStatus;

extern MIBPacket						mib_packet;
extern MIBState 						mib_state;
extern __persistent MIBExecutiveStatus	status;

#endif
