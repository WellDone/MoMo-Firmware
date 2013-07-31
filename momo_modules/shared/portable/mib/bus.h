#ifndef __bus_common_h__
#define __bus_common_h__

#include "platform.h"
#include "mib_hal.h"
#include "i2c.h"
#include "protocol.h"

#define kControllerPICAddress	0x08

//Callback type for master rpc routines
typedef 	void (*mib_rpc_function)(unsigned char);

//Bus error codes that can be returned
enum
{
	kNoMIBError = 0,
	kUnsupportedCommand = 1,
	kWrongParameterType = 2,
	kParameterTooLong = 3,
	kParameterChecksumError = 4,
	kCommandChecksumError = 5,
	kUnknownError = 6,
	kCallbackError = 7,
	kSlaveNotAvailable = 255
};

//Takes 2 bits to store
//Cannot change.  Referenced by mib_hal.as in pic12 code
typedef enum
{
	kMIBIdleState = 0,
	kMIBSearchCommand = 1,
	kMIBFinishCommand = 2,
	kMIBProtocolError = 3,
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
	MIBCommandPacket		bus_command;	//3 bytes
	I2CMessage				bus_msg;		//4 bytes
	MIBReturnValueHeader	bus_returnstatus;//1 byte

	//handlers
#ifndef _PIC12 //TODO: Also on PIC12?
	uint8					feature_index;
#endif
	uint8					slave_handler;	//1 byte
	
	//PIC12 does not support ascynchronous master RPCs
	#ifndef _PIC12
	mib_rpc_function		master_callback;
	#endif

	union 									//1 byte
	{
		struct 
		{
			volatile uint8			num_reads	 : 2;
			volatile uint8			slave_state  : 2; //Referenced by mib_hal.as on pic12, cannot change
			volatile uint8 			master_state : 3;
			volatile uint8			rpc_done 	 : 1;
		};

		volatile uint8 				combined_state;
	};
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
void bus_slave_send(unsigned char* buffer, uint8 len, unsigned char imm);
void bus_slave_receive(unsigned char* buffer, uint8 len, unsigned char imm);
#else

#define bus_slave_send(buffer, len, imm)					\
{															\
	mib_state.bus_msg.address = kInvalidI2CAddress | imm;	\
	mib_state.bus_msg.data_ptr = buffer;					\
	mib_state.bus_msg.last_data = buffer + len;				\
	i2c_slave_send_message();								\
}

#define bus_slave_receive(buffer, len, imm) 				\
{															\
	mib_state.bus_msg.address = kInvalidI2CAddress | imm;	\
	mib_state.bus_msg.data_ptr = buffer;					\
	mib_state.bus_msg.last_data = buffer + len;				\
	i2c_slave_receive_message();							\
}

#define bus_send(add, buffer, len)					\
{													\
	mib_state.bus_msg.address = add;				\
	mib_state.bus_msg.data_ptr = buffer;			\
	mib_state.bus_msg.last_data = buffer + len;		\
	i2c_master_send_message();						\
}

#define bus_receive(add, buffer, length)			\
{													\
	mib_state.bus_msg.address = add;				\
	mib_state.bus_msg.data_ptr = buffer;			\
	mib_state.bus_msg.last_data = buffer + length;	\
	i2c_master_receive_message();					\
}

#endif


#endif