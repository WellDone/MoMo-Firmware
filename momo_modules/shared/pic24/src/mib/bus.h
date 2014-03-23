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

typedef enum
{
	kMIBIdleState = 0,
	kMIBSearchCommand = 1,
	kMIBFinishCommand = 2,
	kMIBProtocolError = 3
} MIBSlaveState;

//Need these because XC8 is really bad at optimizing bit operations
#define shift_master_state(state)		(state << 4)
#define set_master_state(state)			{mib_state.combined_state &= 0b10001111; mib_state.combined_state |= shift_master_state(state);}
#define shift_slave_state(state)		(state << 2)
#define set_slave_state(state)			{mib_state.combined_state &= 0b111110011; mib_state.combined_state |= shift_slave_state(state);}
#define bus_has_returnvalue()			(mib_state.bus_returnstatus.len != 0)
#define bus_get_returnvalue_length()	(mib_state.bus_returnstatus.len)
#define bus_inc_numreads()				(mib_state.combined_state += 1)							//okay since numreads cannot be more than 2 so this won't overflow
#define bus_numreads_odd()				(mib_state.combined_state & 0x01)
#define bus_numreads_full()				((mib_state.combined_state & 0b11) == 0b11)
#define bus_numreads_nonzero()			(mib_state.combined_state & 0b11)

typedef enum 
{
	kMIBSendParameters = 1,
	kMIBReadReturnStatus,
	kMIBReadReturnValue,
	kMIBExecuteCallback,
	kMIBResendCommand,
	kMIBFinalizeMessage
} MIBMasterState;

//10 bytes long
typedef struct 
{
	//Shared Buffers
	I2CMessage				bus_msg;		//4 bytes
	MIBReturnValueHeader	bus_returnstatus;//1 byte

	//handlers
	uint8					feature_index;
	uint8					slave_handler;	//1 byte
	
	mib_rpc_function		master_callback;

	union 									//1 byte
	{
		struct 
		{
			volatile uint8			num_reads	 : 2;
			volatile uint8			slave_state  : 2;
			volatile uint8 			master_state : 3;
			volatile uint8			rpc_done 	 : 1;
		};

		volatile uint8 				combined_state;
	};
} MIBState;

typedef struct
{
	unsigned char 		address;
	MIBCommandPacket	bus_command;	//3 bytes
	unsigned char 		mib_buffer[kBusMaxMessageSize];
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
void bus_slave_send(unsigned char* buffer, uint8 len, unsigned char imm);
void bus_slave_receive(unsigned char* buffer, uint8 len, unsigned char imm);

#endif