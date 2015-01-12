#ifndef __gsmstream_h__
#define __gsmstream_h__

#include "platform.h"

void comm_set_destination( uint8, const char*, uint8 );

void gsm_openstream();
void gsm_putstream();
void gsm_closestream();
void gsm_abandonstream();

#endif