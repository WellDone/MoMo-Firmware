/* definitions.h
 *
 * Application Modules must define the following information appropriate to each module
 * This data is automatically stored in memory in the appropriate location and used by 
 * the MIB executive as needed.  Nothing more needs to be done than simply define it
 */

\#ifndef __definitions_h__
\#define __definitions_h__

\#define __DEFINES_ONLY__
\#include "mib_definitions.h"
\#undef __DEFINES_ONLY__

/* 
 * A readable name for this module
 * Must be exactly 8 characters long and stored comma separated characters
 * with double quotes
 */

\#define ModuleName 			'U','n','n','a','m','e','d','0'

/*
 * The module class
 * Should be a valid option defined in mib_definitions.h
 */

\#define ModuleType			kMIBSensorType


/*
 * The module application code version
 * A version number that is meaningful for the application code on this module
 */

\#define ModuleVersion 		1

/*
 * Module special flags
 * Indicate if your module supports any defined special mib features here
 */

\#define ModuleFlags			0

\#endif