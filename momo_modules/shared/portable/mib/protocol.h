#ifndef __protocol_h__
#define __protocol_h__

#include "platform.h"

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

//Macros for defining parameter lists
#define plist_ints(count)		((count&0b11) << 5)
#define plist_buffer()          0b10000000

#define plist_spec_empty()      0
#define plist_spec(ints,buffer) (((buffer)? plist_buffer() : 0) | plist_ints(ints) )
#define plist_spec_mask         0b11100000
#define plist_buffer_mask		0b00011111

#define plist_with_buffer(ints,buffer_length) (plist_spec(ints, 1)|(buffer_length&0x1F))
#define plist_no_buffer(ints)	plist_ints(ints)
#define plist_empty()			plist_spec_empty()

#define plist_matches(plist,spec)     ((plist & plist_spec_mask) == spec)

#define plist_set_int16(n, val)			((int*)mib_buffer)[n] = val
#define plist_set_int8(n, hi, val)		mib_buffer[(n<<1) + hi] = val
#define plist_get_int16(n)				((int*)mib_buffer)[n]
#define plist_get_int8(n)				mib_buffer[n<<1]
#define plist_get_buffer(n)				(mib_buffer + (n << 1))
#define plist_get_buffer_length()		(mib_state.bus_command.param_spec & 0b00011111)

#define pack_return_status(status, return_length)	(((status & 0b111) << 5) | (return_length & 0b00011111))

#endif