#ifndef __common_types_h__
#define __common_types_h__

#include <GenericTypeDefs.h>
#include "bit_utilities.h"

typedef unsigned short bool;
#define false 0
#define true 1

typedef signed char 	int8;
typedef unsigned char	uint8;
#define MAX_UINT8 255

typedef signed int      int16;
typedef unsigned int    uint16;
#define MAX_UINT16 65535

typedef signed long     int32;
typedef unsigned long   uint32;
#define MAX_UINT32 4294967295L

typedef signed long long int64;
typedef unsigned long long uint64;

inline uint16 makeu16( uint8 high, uint8 low );
inline uint16 make16( int8 high, int8 low );
inline uint32 makeu32( uint16 high, uint16 low );
inline uint32 make32( int16 high, int16 low );

#endif