#ifndef __protocol_h__
#define __protocol_h__

#include "platform.h"

//Configurable defines
#define kMIBCommandLength 	sizeof(MIBCommandPacket)
#define kBusMaxMessageSize 	20

enum
{
	kMIBInt16Type = 0,
	kMIBBufferType = 1
};

typedef struct
{
	unsigned char feature;
	unsigned char command;
	unsigned char param_length;
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
	unsigned char 	   data[1]; /*data follows struct*/
} MIBBufferParameter;

typedef struct 
{
	unsigned char result;
	unsigned char len;
} MIBReturnValueHeader;

//Slave endpoint handler type
typedef void (*mib_callback)(void);

//Macros for defining parameter lists
#define plist_param_n(n, type) 					((type & 0x01) << (n+3))
#define plist_1param(type) 						plist_param_n(0, type)
#define plist_2params(type1, type2) 			(plist_param_n(0, type1) | plist_param_n(1, type2))
#define plist_3params(type1, type2, type3) 		(plist_param_n(0, type1) | plist_param_n(1, type2) | plist_param_n(2, type3))
#define plist_define(count, params) 			((count & 0b111) | params)

#define plist_define0()							plist_define(0, 0)
#define plist_define1(type)						plist_define(1, plist_1param(type))
#define plist_define2(type1, type2)				plist_define(2, plist_2params(type1, type2))
#define plist_define3(type1, type2, type3)		plist_define(3, plist_3params(type1, type2, type3))

#define extract_param_type(params, n) ((params & (1<<(n+3))) >> (n+3))
#define extract_param_count(params) (params & 0b111)

//we know parameter list is all ints and at most one buffer at the end, so we can advance by one int param each time
#define advance_param_ptr(ptr)	((unsigned char *)(ptr))+=sizeof(MIBIntParameter)

#define get_int16_param(n) ((int)(((MIBIntParameter*)mib_buffer)[n].value))
#define get_uint16_param(n) ((unsigned int)(((MIBIntParameter*)mib_buffer)[n].value))
#define get_buffer_param(n) ((MIBBufferParameter*)&(((MIBIntParameter*)mib_buffer)[n]))

#endif