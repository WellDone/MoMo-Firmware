#ifndef __protocol_h__
#define __protocol_h__

enum
{
	kMIBInt16Type = 0,
	kMIBBufferType = 1
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
	
	MIBParameterHeader *params[1]; //variable length array, use bus_allocate_param_list to create this structure
} MIBParamList;

typedef struct 
{
	unsigned char result;
	unsigned char len;
} MIBReturnValueHeader;

//Callback Type
typedef void* (*mib_callback)(MIBParamList *);

#define kMIBCommandLength 	sizeof(MIBCommandPacket)
#define kBusMaxMessageSize 	32

#define get_int16_param(list, n) ((int)((MIBIntParameter *)list->params[n])->value)
#define get_uint16_param(list, n) ((unsigned int)((MIBIntParameter *)list->params[n])->value)
#define get_buffer_param(list, n) ((MIBBufferParameter*)list->params[n])

#endif