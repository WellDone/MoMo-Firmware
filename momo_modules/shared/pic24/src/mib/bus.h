#ifndef __bus_common_h__
#define __bus_common_h__

#include "platform.h"
#include "mib_hal.h"
#include "i2c.h"
#include "protocol.h"

//Bus error codes that can be returned
enum
{
	kNoMIBError = 0,
	kUnsupportedCommand = 1,
	kWrongParameterType = 2,
	kParameterTooLong = 3,
	kChecksumError = 4,
	kUnknownError = 6,
	kCallbackError = 7,
	kSlaveNotAvailable = 255
};

//Need these because XC8 is really bad at optimizing bit operations
#define set_master_state(state)			mib_state.master_state = state
#define set_slave_state(state)			mib_state.slave_state = state
#define bus_has_returnvalue()			(mib_state.bus_returnstatus.len != 0)
#define bus_get_returnvalue_length()	(mib_unified.bus_returnstatus.len)

typedef enum 
{
	kMIBIdleState = 0,
	kMIBSendParameters,
	kMIBReadReturnStatus,
	kMIBReadReturnValue,
	kMIBExecuteCallback,
	kMIBFinalizeMessage
} MIBMasterState;

typedef struct 
{
	//Shared Buffers
	I2CMessage				bus_msg;		//4 bytes

	//handlers
	uint8					feature_index;
	uint8					slave_handler;	//1 byte
	
	mib_rpc_function		master_callback;

	volatile uint8			first_read;
	volatile uint8 			master_state;
	volatile uint8			rpc_done;
} MIBState;

typedef struct
{
	unsigned char 			address;
	union
	{
		MIBCommandPacket	bus_command;	//3 bytes
		struct
		{
			unsigned char 		 padding;
			MIBReturnValueHeader bus_returnstatus;//1 byte
			unsigned char		 status_checksum;
		};
	};

	unsigned char 		mib_buffer[kBusMaxMessageSize];
	unsigned char		buffer_checksum;	//need potentially one more byte for checksum if mib_buffer is completely full
} MIBUnified; 

/*
 * Make sure files have access to the fundamental MIB global state variables so that
 * we can replace functions with macros that reference these variables.
 */

#ifndef __NO_EXTERN_MIB_STATE__

#include "mib_state.h"

#endif

uint8 plist_param_length(uint8 plist);

//Bus transmission functions
void bus_send(unsigned char address, unsigned char *buffer, unsigned char len);
void bus_receive(unsigned char address, unsigned char *buffer, unsigned char len);
void bus_slave_send(unsigned char* buffer, uint8 len);
void bus_slave_receive(unsigned char* buffer, uint8 len);

void bus_append_checksum(unsigned char *buffer, unsigned int length);

#endif