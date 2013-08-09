/* mib_definitions.h
 * ALl of the MIB related structures and constant values shared between various mib
 * modules are stored here.  All declarations that are not constant values using #define
 * must be surrounded by #ifndef __DEFINES_ONLY__ guards to allow for this file to be
 * included by asm modules.
 */

#ifndef __mib_definitions_h__
#define __mib_definitions_h__

#ifndef __DEFINES_ONLY__
#include "common_types.h"
#endif

//Well-Known MIB Addresses
#define kMIBControllerAddress		8
#define kMIBUnenumeratedAddress		127

//Defined MIB Module Types
#define kMIBControllerType 			1
#define kMIBCommunicationType		2
#define kMIBGenericType				3
#define kMIBExecutiveOnlyType		4
#define kMIBSensorType				5

//Defined MIB Hardware Types
#define kMIBPic24f16ka101			1
#define kMIBPic12lf1822				2
#define kMIBPic16lf1823				3

//Defined MIB Protocol revisions
#define kMIBVersion1				1

//Defined Flags

/*
 * The structure returned by a mib module upon registration, describing itself
 * Fields have the following meanings:
 * module_type - a module class specific identifier
 * hardware_type - the type of processor this application is running on
 * mib_revision - the version of the mib protocol that this module speaks
 * feature_count - the number of features supported by the module
 * flags - a bit array of special interest flags about the module
 * name - an 8 byte string with a user readable name for the module
 */

#ifndef __DEFINES_ONLY__
typedef struct 
{
	uint8 	module_type;
	uint8	hardware_type;
	uint8	mib_revision;
	uint8	feature_count;
	uint8	flags;
	uint8 	name[8];
} momo_module_descriptor;
#endif

#endif