/*
 * The MIB Protocol v2.0
 * This file contains all of the information needed to speak and understand the MoMo 
 * remote procedure call protocol, known as the MIB (Module Interface Bus).
 * 
 * MIB calls are structured as fixed sized packets where a master sends a command
 * packet to a slave to initiate an RPC, the slave executes the function and then
 * the master reads a response packet containing the results.  
 *
 * Version 2.0 of the MIB Protocol adds the following features over the first version:
 * - Support for asynchronous RPC calls wheere the bus is not locked while the slave
 *   executes the callback.  Instead the slave makes a special master call to notify the
 *   original caller of the result of the RPC.
 * 
 * - Unified packet structure to make for simpler implementation on low-performance 8-bit
 *   devices. 
 *
 * - A fixed timeout for RPC calls so that the bus can never be locked forever if a slave
 *   receives an RPC call but never gets around to processing it.
 */

#ifndef __protocol_structures_h__
#define __protocol_structures_h__

#include <stdint.h>
#include "protocol_defines.h"

//On-the-wire Structures
typedef struct
{
	union
	{
		struct
		{
			uint8_t 	reserved1: 1;
			uint8_t 	reserved2: 1;
			uint8_t		reserved3: 1;
			uint8_t		length: 5;
		};

		uint8_t flags_and_length;
	};

	uint8_t 	sender;
	uint16_t 	command;
} MIBCommandHeader;

union
{
	struct
	{
		uint8_t has_data: 1;
		uint8_t app_defined: 1;
		uint8_t code: 6;	
	};

	uint8_t value;
};

typedef struct 
{
	union
	{
		struct
		{
			uint8_t has_data: 1;
			uint8_t app_defined: 1;
			uint8_t status_code: 6;	
		};

		uint8_t status_value;
	};

	uint8_t 		status_checksum;
	uint8_t			reserved;
	uint8_t			length;
} MIBResponseHeader;

/*
 * MIBPacket v2.0
 * All data exchanged on the mib bus is just a serialization of this structure
 * RPC calls use the MIBCommandHeader type header and the response they receive
 * contains a MIBResponseHeader type header.  Both are followed by a fixed length
 * buffer of data bytes.
 */
typedef struct
{
	union
	{
		MIBCommandHeader 	call;
		MIBResponseHeader	response;
	};

	uint8_t data[kMIBBufferSize];
	uint8_t checksum;
} MIBPacket;

#endif
