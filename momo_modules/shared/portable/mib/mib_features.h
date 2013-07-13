#ifndef __mib_features_h
#define __mib_features_h

#include "mib_feature.h"

IMPORT_MIB_FEATURE(test);
IMPORT_MIB_FEATURE(controller);
IMPORT_MIB_FEATURE(programming);

enum {
	MIB_FEATURE_ID(test) = 0,
	MIB_FEATURE_ID(controller) = 42,
	MIB_FEATURE_ID(programming) = 255
};

#endif