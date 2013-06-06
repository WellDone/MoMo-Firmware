
/* bus module
 * This module implements routines and definitions related to the Momo Intermodule Bus (MIB)
 */

#ifndef __bus_h__
#define __bus_h__

#include "common.h"
#include "task_manager.h"
#include "i2c.h"

#define kBusMaxMessageSize 		40
#define kControllerPICAddress	0x08

//Wellknown Command States
enum
{
	kCommandBegin = 0,
	kCommandFinished = 127
};

//Callback Type
typedef int (*mib_callback)(int);

#define kMIBCommandLength 	sizeof(MIBCommandPacket)
#define kInvalidMIBHandler  (mib_callback)0

#define bus_slave_send(buffer, len, flags)			bus_send(kInvalidI2CAddress, buffer, len, flags)
#define bus_slave_receive(buffer, len, flags) 		bus_receive(kInvalidI2CAddress, buffer, len, flags)

typedef struct
{
	unsigned char feature;
	unsigned char command;
} MIBCommandPacket;

typedef struct 
{
	//Slave section
	I2CMessage				slave_msg;
	mib_callback			slave_subhandler;
	int 					slave_substate;

	mib_callback			slave_handler;
	int						slave_state;

	MIBCommandPacket		slave_command;

	//Master section	
	I2CMessage				master_msg;
	mib_callback			subcommand_handler;
	int 					subcommand_state;

	mib_callback			master_handler;
	int						master_state;
} MIBState;

void bus_init();

//Command Routines
int 			bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags);
int 			bus_receive(unsigned char address, volatile unsigned char *buffer, unsigned char len, unsigned char flags);

//Master Routines
int 			bus_master_sendcommand(unsigned char address, MIBCommandPacket *cmd, mib_callback callback, unsigned char *response);
unsigned char 	bus_master_lastaddress();
void 			bus_master_callback();

//Slave Routines
void			bus_slave_startcommand();
void 			bus_slave_callback();


#endif