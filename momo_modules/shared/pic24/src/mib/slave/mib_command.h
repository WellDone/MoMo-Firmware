#ifndef __mib_command_h__
#define __mib_command_h__

#include "protocol.h"

#define kInvalidMIBHandler 255

uint8 	find_handler(unsigned char feature, unsigned char cmd);
uint8	build_params(uint8 handler_index);

#endif