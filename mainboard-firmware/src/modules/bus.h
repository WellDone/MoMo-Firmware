/* bus module
 * This module implements routines and definitions related to the Momo Intermodule Bus (MIB)
 */

#ifndef __bus_h__
#define __bus_h__

#include "common.h"
#include "task_manager.h"
#include "i2c.h"

typedef enum  
{
	kIdleState = 0,
	kReceivingCommandState,
	kReceivedCommandState,
	kProcessedCommandState,
	kExecutingCommandState,
	kErrorState
} SlaveProcessingState;

typedef enum  
{
	kNoI2CError = 0,
	kMalformedCommandPacketError,
	kUnsupportedCommandError,
	kUnsupportedFeatureError,
	kProtocolError
} SlaveError;

typedef enum
{
	kInitiatingCommand = 0,
	kTerminatingCommand,
	kUsercodeExecuting,
	kReceivingBuffer,
	kSendingBuffer
} MIBCommandState;

//Callback Type
typedef void (*mib_callback)(unsigned int);

#define kMIBCommandLength 	sizeof(MIBCommandPacket)

typedef union
{
	unsigned char	  bytes[3];

	struct {
		unsigned char feature;
		unsigned char command;
		unsigned char checksum;
	} sep;
} MIBCommandPacket;

typedef struct 
{
	SlaveProcessingState 	receive_state;
	SlaveError				last_error;

	MIBCommandPacket 	 	curr_cmd;
	unsigned int 			curr_cmd_byte;

	//Command Execution Data
	mib_callback			command_continuation; 	//the handler that processes this command
	unsigned char			received_byte; 			//The last byte we received from the wire
	MIBCommandState			command_state;
} MIBState;

void bus_init();

#endif