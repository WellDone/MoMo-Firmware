
/* bus module
 * This module implements routines and definitions related to the Momo Intermodule Bus (MIB)
 */

#ifndef __bus_h__
#define __bus_h__

#include "common.h"
#include "task_manager.h"
#include "i2c.h"
#include "protocol.h"
#include "mib_command.h"

#define kControllerPICAddress	0x08


#define bus_slave_send(buffer, len, flags)			bus_send(kInvalidI2CAddress, buffer, len, flags)
#define bus_slave_receive(buffer, len, flags) 		bus_receive(kInvalidI2CAddress, buffer, len, flags)

enum
{
	kNoMIBError = 0,
	kUnsupportedCommand = 1,
	kWrongParameterType,
	kParameterTooLong,
	kWrongChecksum
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
	kMIBFinalizeMessage
} MIBMasterState;

typedef struct 
{
	//Slave section
	I2CMessage				slave_msg;
	mib_callback			slave_handler;
	MIBSlaveState			slave_state;
	int 					num_reads;
	MIBReturnValueHeader	slave_returnstatus;

	MIBCommandPacket		slave_command;
	MIBParameterHeader 		last_param;
	MIBParamList			*slave_params;

	//Master section	
	I2CMessage				master_msg;
	MIBMasterState			master_state;
	MIBCommandPacket		master_command;
	unsigned char 			master_param_length;
	unsigned char 			master_result;

} MIBState;

void bus_init();

//Command Routines
int 			bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags);
int 			bus_receive(unsigned char address, volatile unsigned char *buffer, unsigned char len, unsigned char flags);

//Master Routines
unsigned char 	bus_master_lastaddress();
void 			bus_master_callback();
int 			bus_master_rpc(unsigned char address, unsigned char feature, unsigned char cmd, MIBParameterHeader **params, unsigned char param_count);

//Slave Routines
void			bus_slave_startcommand();
void 			bus_slave_callback();
void 			bus_slave_seterror(unsigned char error);
void			bus_slave_setreturn(unsigned char status, volatile MIBParameterHeader *value);

#endif