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

#define kNumFeatures 2
#define kNumCommands 5

const unsigned char features[kNumFeatures] = {2, 255};
const unsigned char commands[kNumFeatures+1] = {0,2, kNumCommands};
const mib_callback  handlers[kNumCommands] = {echo_buffer, test_command, //end feature 2
											  erase_primaryfirmware, load_into_nvram, read_from_nvram //end feature 255
											};
const unsigned char param_specs[kNumCommands] = {
												plist_define1(kMIBBufferType), //echo_buffer
												plist_define2(kMIBInt16Type, kMIBInt16Type), //test command
												plist_define0(),
												plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBBufferType),
												plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBInt16Type)
												};

#endif