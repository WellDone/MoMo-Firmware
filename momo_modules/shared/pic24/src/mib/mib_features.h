/*
HERE THERE BE EXTERNS
DO NOT include this in a file which defines a feature, include mib_feature_definition.h instead.
*/

#include "mib_feature.h"
#ifndef NO_MIB_FEATURE_EXTERNS


IMPORT_MIB_FEATURE(async);
IMPORT_MIB_FEATURE(controller);
IMPORT_MIB_FEATURE(firmware_cache);
IMPORT_MIB_FEATURE(reporting);
IMPORT_MIB_FEATURE(config);
IMPORT_MIB_FEATURE(sensor);
IMPORT_MIB_FEATURE(scheduler);


#endif
#ifndef __mib_features_h
#define __mib_features_h


enum {
	MIB_FEATURE_ID(async) = 0,
	MIB_FEATURE_ID(firmware_cache) = 7,
	MIB_FEATURE_ID(controller) = 42,
	MIB_FEATURE_ID(scheduler) = 43,
	MIB_FEATURE_ID(reporting) = 60,
	MIB_FEATURE_ID(config) = 61,
	MIB_FEATURE_ID(sensor) = 70
};


#endif