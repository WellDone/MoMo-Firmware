#ifndef __mib_command_h__
#define __mib_command_h__

#include "protocol.h"

#define kInvalidMIBIndex 255

unsigned char			find_handler(unsigned char feature, unsigned char cmd);
uint8					build_params(unsigned char handler_index);					

#endif