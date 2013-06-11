#ifndef __protocol_h__
#define __protocol_h__

#include "common.h"

enum
{
	kMIBInt16Type,
	kMIBBufferType
};

typedef struct
{
	unsigned char feature;
	unsigned char command;
} MIBCommandPacket;

typedef struct
{
	unsigned char type;
	unsigned char len;
} MIBParameterHeader;

typedef struct
{
	MIBParameterHeader header;
	int 		  	  value;
} MIBIntParameter;

typedef struct
{
	MIBParameterHeader header;
	unsigned char 	   *data;
} MIBBufferParameter;

typedef struct 
{
	unsigned char count;
	unsigned char curr;
	
	MIBParameterHeader *params[0]; //variable length array, use bus_allocate_param_list to create this structure
} MIBParamList;

typedef struct 
{
	unsigned char result;
	unsigned char len;
} MIBReturnValueHeader;

#define kMIBCommandLength 	sizeof(MIBCommandPacket)
#define kBusMaxMessageSize 	40

//Allocation Routines
volatile unsigned char * 	bus_allocate_space(unsigned int len);
void						bus_free_all();

volatile MIBParamList		*bus_allocate_param_list(unsigned int num);
volatile MIBIntParameter 	*bus_allocate_int_param();
volatile MIBBufferParameter *bus_allocate_buffer_param(unsigned int len);
volatile MIBReturnValueHeader *bus_allocate_return_status();

void 						bus_init_int_param(MIBIntParameter *param, int value);
void 						bus_init_buffer_param(MIBBufferParameter *param, void *data, unsigned char len);

#endif