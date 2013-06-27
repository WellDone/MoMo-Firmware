#ifndef __prog_h
#define __prog_h

#include "protocol.h"

void* erase_primaryfirmware(MIBParamList *param);
void* load_into_nvram(MIBParamList *param);
void* read_from_nvram(MIBParamList *param);

#endif