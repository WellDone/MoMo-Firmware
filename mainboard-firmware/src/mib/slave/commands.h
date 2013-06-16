//commands.h

/*
 * NB this file should not be included.  It defines the mib command map and is automatically included
 * in mib_command.h
 */

#ifndef __commands_h__
#define __commands_h__

#include "protocol.h"

//include files for all the commands we support
#include "test.h"
#include "prog.h"

//Macros for defining parameter lists
#define plist_param_n(n, type) ((type & 0x01) << (n+3))
#define plist_1param(type) plist_param_n(0, type)
#define plist_2params(type1, type2) (plist_param_n(0, type1) | plist_param_n(1, type2))
#define plist_define(count, params) ((count & 0b111) | params)


#define kNumFeatures 2
#define kNumCommands 4
const unsigned char features[kNumFeatures] = {2, 255};
const unsigned char commands[kNumFeatures+1] = {0,2, kNumCommands};
const mib_callback  handlers[kNumCommands] = {echo_buffer, test_command, //end feature 2
											  erase_nvbuffer, load_nvbuffer //end feature 255
											};
const unsigned char param_specs[kNumCommands] = {
												plist_define(1, plist_1param(kMIBBufferType)), //echo_buffer
												plist_define(2, plist_2params(kMIBInt16Type, kMIBInt16Type)), //test command
												0,
												0
												};

#endif