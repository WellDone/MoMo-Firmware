#ifndef __mib_feature_h
#define __mib_feature_h

#include "mib_command.h"
#include "mib_command_parameters.h"

#define MIB_FEATURE_CMD_MAP(name) mib_feature_##name##_commands
#define MIB_FEATURE_OBJ(name) mib_feature_##name
#define MIB_FEATURE(name) &MIB_FEATURE_OBJ(name)
#define MIB_FEATURE_ID(name) kMib##name##Feature

#define DEFINE_MIB_FEATURE_COMMANDS(name) static mib_command_handler MIB_FEATURE_CMD_MAP(name) [] = 
#define MIB_FEATURE_CMD_COUNT(name) (sizeof(MIB_FEATURE_CMD_MAP(name))/sizeof(mib_command_handler))

#define DEFINE_MIB_FEATURE(name) feature_map MIB_FEATURE_OBJ(name) = {MIB_FEATURE_ID(name), MIB_FEATURE_CMD_MAP(name), MIB_FEATURE_CMD_COUNT(name)};
								 //void dummy_mib_##name##_importer() { feature_map* a = &MIB_FEATURE_OBJ(name); }
#define IMPORT_MIB_FEATURE(name) extern feature_map MIB_FEATURE_OBJ(name)

#define DEFINE_FEATURE_MAP() static const feature_map* the_mib_features[] = 
#define REGISTER_FEATURE_MAP() register_mib_features(the_mib_features, sizeof(the_mib_features)/sizeof(feature_map) )

#endif