#ifndef __port_inc__
#define __port_inc__

#include <xc.inc>

#define PORT_r(name)		PORT ## name
#define LAT_r(name)			LAT ## name
#define TRIS_r(name)		TRIS ## name

#define PORT(name)			PORT_r(name)
#define LAT(name)			LAT_r(name)
#define TRIS(name)			TRIS_r(name)

#endif
