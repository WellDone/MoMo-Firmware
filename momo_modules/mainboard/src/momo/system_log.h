#ifndef __system_log_h__
#define __system_log_h__

/*
	Debug is the only mode.
*/

#include "platform.h"

void init_system_log( uint8 start_subsection, uint8 subsection_count );
void write_system_log( const char* data, uint8 length );

#endif