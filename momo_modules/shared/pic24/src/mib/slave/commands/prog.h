#ifndef __prog_h
#define __prog_h

#include "protocol.h"

void erase_primaryfirmware(void);
void load_into_nvram(void);
void read_from_nvram(void);

#endif