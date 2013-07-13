#ifndef __bus_common_h__
#define __bus_common_h__

#include "platform.h"
#include "mib_hal.h"
#include "i2c.h"
#include "protocol.h"

#define kControllerPICAddress	0x08

#define bus_slave_send(buffer, len, flags)			bus_send(kInvalidI2CAddress, buffer, len, flags)
#define bus_slave_receive(buffer, len, flags) 		bus_receive(kInvalidI2CAddress, buffer, len, flags)

//Callback type for master rpc routines
typedef 	void (*mib_rpc_function)(unsigned char);

//Bus error codes that can be returned
enum
{
	kNoMIBError = 0,
	kUnsupportedCommand,
	kWrongParameterType,
	kParameterTooLong,
	kParameterChecksumError,
	kCommandChecksumError,
	kUnknownError,
	kSlaveNotAvailable = 255
};

typedef enum
{
	kMIBIdleState = 0,
	kMIBSearchCommand,
	kMIBReceiveParameterHeader,
	kMIBReceiveParameterValue,
	kMIBFinishedReceivingParameters,
	kMIBReceivedParameterChecksum,
	kMIBExecuteCommandHandler,
	kMIBFinishCommand,
	kMIBProtocolError,
} MIBSlaveState;

typedef enum 
{
	kMIBSendParameters = 1,
	kMIBReadReturnStatus,
	kMIBReadReturnValue,
	kMIBExecuteCallback,
	kMIBResendCommand,
	kMIBFinalizeMessage
} MIBMasterState;

typedef struct 
{
	//Shared Buffers
	MIBCommandPacket		bus_command;
	I2CMessage				bus_msg;
	MIBReturnValueHeader	bus_returnstatus;

	//Slave section
	uint8					slave_handler;
	volatile MIBSlaveState	slave_state;
	uint8 					num_reads;

	//Master section	
	volatile MIBMasterState master_state;
	mib_rpc_function		master_callback;
} MIBState;

/*
 * Make sure files have access to the fundamental MIB global state variables so that
 * we can replace functions with macros that reference these variables.
 */

#ifndef __NO_EXTERN_MIB_STATE__

extern MIBState 			mib_state;
extern bank1 unsigned char 	mib_buffer[kBusMaxMessageSize];
extern bank1 unsigned char 	mib_firstfree;

#endif

//Bus transmission functions
#ifndef _MACRO_SMALL_FUNCTIONS
void bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags);
void bus_receive(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags);
#else

#define bus_send(add, buffer, len, f)				\
{													\
	mib_state.bus_msg.address = add;				\
	mib_state.bus_msg.data_ptr = buffer;			\
	mib_state.bus_msg.last_data = buffer + len;		\
	mib_state.bus_msg.flags = f;					\
	i2c_send_message();								\
}

#define bus_receive(add, buffer, length, f)			\
{													\
	mib_state.bus_msg.address = add;				\
	mib_state.bus_msg.data_ptr = buffer;			\
	mib_state.bus_msg.last_data = buffer;			\
	mib_state.bus_msg.len = length;					\
	mib_state.bus_msg.flags = f;					\
	i2c_receive_message();							\
}

#endif


#endif