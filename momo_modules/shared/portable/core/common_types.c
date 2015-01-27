#include "common_types.h"

inline uint16 makeu16( uint8 high, uint8 low ) {
	return (((uint16)high << 8) | low);
}
inline uint16 make16( int8 high, int8 low ) {
	return (((uint16)high << 8) | low);
}
inline uint32 makeu32( uint16 high, uint16 low ) {
	return (((uint32)high << 16) | low);
}
inline uint32 make32( int16 high, int16 low ) {
	return (((uint32)high << 16) | low);
}