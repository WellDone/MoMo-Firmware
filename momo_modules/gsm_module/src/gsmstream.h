#ifndef __gsmstream_h__
#define __gsmstream_h__

#include "platform.h"

uint8_t set_comm_destination();
char  comm_destination_get(uint8_t offset);

uint8_t gsm_openstream();
uint8_t gsm_putstream();
uint8_t gsm_closestream();

#endif