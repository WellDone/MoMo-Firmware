#ifndef __gsmstream_h__
#define __gsmstream_h__

#include "platform.h"

uint8 set_comm_destination();
char  comm_destination_get(uint8 offset);

void gsm_openstream();
void gsm_putstream();
void gsm_closestream();
void gsm_abandonstream();



#endif