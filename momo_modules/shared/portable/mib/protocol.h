#ifndef __protocol_h__
#define __protocol_h__

#include "platform.h"
#include "mib_definitions.h"

//Configurable defines
#define kMIBCommandLength 	sizeof(MIBCommandPacket)
#define kBusMaxMessageSize 	20
#define kInvalidMIBIndex 	255

#define kIntSize 2

enum
{
	kMIBInt16Type = 0,
	kMIBBufferType = 1
};

typedef struct
{
	unsigned char feature;
	unsigned char command;
	unsigned char param_spec;
} MIBCommandPacket;

typedef struct 
{
	union 
	{
		struct
		{
			unsigned char len: 5;
			unsigned char result: 3;
		};
		unsigned char return_status;
	};
} MIBReturnValueHeader;

//Slave endpoint handler type
typedef void (*mib_callback)(void);


#endif