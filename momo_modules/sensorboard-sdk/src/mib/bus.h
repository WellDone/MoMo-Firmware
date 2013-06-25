
/* bus module
 * This module implements routines and definitions related to the Momo Intermodule Bus (MIB)
 */

#ifndef __bus_h__
#define __bus_h__

#include "common.h"
//#include "task_manager.h"
#include "i2c.h"
#include "protocol.h"
#include "mib_command.h"

#define kControllerPICAddress	0x08

#define bus_slave_send(buffer, len, flags)			bus_send(kInvalidI2CAddress, buffer, len, flags)
#define bus_slave_receive(buffer, len, flags) 		bus_receive(kInvalidI2CAddress, buffer, len, flags)

//Callback type for master rpc routines
typedef 	void (*mib_rpc_function)(unsigned char, MIBParameterHeader *);

//Bus error codes that can be returned
enum
{
	kNoMIBError = 0,
	kUnsupportedCommand,
	kWrongParameterType,
	kParameterTooLong,
	kParameterChecksumError,
	kCommandChecksumError,
	kParameterListNotBuilt,
	kUnknownError
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
	I2CMessage				bus_msg;
	MIBCommandPacket		bus_command;
	MIBReturnValueHeader	bus_returnstatus;

	//Slave section
	mib_callback			slave_handler;
	MIBSlaveState			slave_state;
	int 					num_reads;

	MIBParameterHeader 		last_param;
	MIBParamList			*slave_params;

	//Master section	
	MIBMasterState			master_state;
	unsigned char 			master_param_length;
	mib_rpc_function		master_callback;
} MIBState;

//Configuration Routines
void bus_init();

//Allocation Routines
volatile unsigned char * 		bus_allocate_space(unsigned int len);
void							bus_free_all();

volatile MIBParamList		*	bus_allocate_param_list(unsigned int num);
volatile MIBIntParameter 	*	bus_allocate_int_param();
volatile MIBBufferParameter *	bus_allocate_buffer_param(unsigned int len); //if len == 0, then allocate all remaining space
MIBParameterHeader *			bus_allocate_return_buffer(unsigned char **out_buffer);

void 							bus_init_int_param(MIBIntParameter *param, int value);
void 							bus_init_buffer_param(MIBBufferParameter *param, void *data, unsigned char len);

//Command Routines
int bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags);
int bus_receive(unsigned char address, volatile unsigned char *buffer, unsigned char len, unsigned char flags);

#endif
