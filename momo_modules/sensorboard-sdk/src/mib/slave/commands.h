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
//#include "prog.h"

//Macros for defining parameter lists
#define plist_param_n(n, type) 					((type & 0x01) << (n+3))
#define plist_1param(type) 						plist_param_n(0, type)
#define plist_2params(type1, type2) 			(plist_param_n(0, type1) | plist_param_n(1, type2))
#define plist_3params(type1, type2, type3) 		(plist_param_n(0, type1) | plist_param_n(1, type2) | plist_param_n(2, type3))
#define plist_define(count, params) 			((count & 0b111) | params)

#define plist_define0()							plist_define(0, 0)
#define plist_define1(type)						plist_define(1, plist_1param(type))
#define plist_define2(type1, type2)				plist_define(2, plist_2params(type1, type2))
#define plist_define3(type1, type2, type3)		plist_define(3, plist_3params(type1, type2, type3))


#define kNumFeatures 1
#define kNumCommands 2
const unsigned char features[kNumFeatures] = {2}; //, 255};
const unsigned char commands[kNumFeatures+1] = {0, kNumCommands}; //2, kNumCommands};
const mib_callback  handlers[kNumCommands] = {blink_pin, blink_pin//, //end feature 2
											  //erase_primaryfirmware, load_into_nvram, read_from_nvram //end feature 255
											};
const unsigned char param_specs[kNumCommands] = {
												plist_define1(kMIBInt16Type), plist_define1(kMIBInt16Type)
												//plist_define0(),
												//plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBBufferType),
												//plist_define3(kMIBInt16Type, kMIBInt16Type, kMIBInt16Type)
												};

#endif