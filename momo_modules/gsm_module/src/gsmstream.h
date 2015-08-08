#ifndef __gsmstream_h__
#define __gsmstream_h__

#include "platform.h"
#include "global_state.h"

uint8_t set_comm_destination();
char  comm_destination_get(uint8_t offset);

GSMError gsm_openstream();
GSMError gsm_putstream();
GSMError gsm_closestream();

#endif