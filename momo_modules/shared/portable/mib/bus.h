#ifndef __bus_common_h__
#define __bus_common_h__

#include "platform.h"
#include "mib_hal.h"
#include "i2c.h"
#include "protocol.h"

#define kControllerPICAddress	0x08

#define bus_slave_send(buffer, len, imm)			bus_send(kInvalidI2CAddress | imm, buffer, len)
#define bus_slave_receive(buffer, len, imm) 		bus_receive(kInvalidI2CAddress | imm, buffer, len)

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

//Takes 2 bits to store
typedef enum
{
	kMIBIdleState = 0,
	kMIBSearchCommand = 1,
	kMIBFinishCommand = 2,
	kMIBProtocolError = 3,
} MIBSlaveState;

//Takes 3 bits to store
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

	//handlers
#ifndef _PIC12 //TODO: Also on PIC12?
	uint8					feature_index;
#endif
	uint8					slave_handler;
	
	//PIC12 does not support ascynchronous master RPCs
	#ifndef _PIC12
	mib_rpc_function		master_callback;
	#endif

	volatile uint8			num_reads	 : 2;
	volatile uint8			slave_state  : 2;
	volatile uint8 			master_state : 3;
	volatile uint8			rpc_done 	 : 1;
} MIBState;

/*
 * Make sure files have access to the fundamental MIB global state variables so that
 * we can replace functions with macros that reference these variables.
 */

#ifndef __NO_EXTERN_MIB_STATE__

extern bank1 MIBState 		mib_state;
extern bank1 unsigned char 	mib_buffer[kBusMaxMessageSize];

#endif

//Bus transmission functions
#ifndef _MACRO_SMALL_FUNCTIONS
void bus_send(unsigned char address, unsigned char *buffer, unsigned char len);
void bus_receive(unsigned char address, unsigned char *buffer, unsigned char len);
#else

#define bus_send(add, buffer, len)					\
{													\
	mib_state.bus_msg.address = add;				\
	mib_state.bus_msg.data_ptr = buffer;			\
	mib_state.bus_msg.last_data = buffer + len;		\
	i2c_send_message();								\
}

#define bus_receive(add, buffer, length)			\
{													\
	mib_state.bus_msg.address = add;				\
	mib_state.bus_msg.data_ptr = buffer;			\
	mib_state.bus_msg.last_data = buffer + length;	\
	i2c_receive_message();							\
}

#endif


#endif