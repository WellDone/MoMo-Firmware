#ifndef __mib_feature_h
#define __mib_feature_h

#include "protocol.h"
#include <stdint.h>

typedef uint8_t (*mib_callback)(uint8_t length);

typedef struct {
	unsigned char id;
	mib_callback handler;
} mib_command_handler;

typedef struct {
	unsigned char id;
	unsigned short command_count;
	mib_command_handler* commands;
} feature_map;

#define MIB_FEATURE_CMD_MAP(name) mib_feature_##name##_commands
#define MIB_FEATURE_OBJ(name) mib_feature_##name
#define MIB_FEATURE(name) &MIB_FEATURE_OBJ(name)
#define MIB_FEATURE_ID(name) kMib##name##Feature

#define DEFINE_MIB_FEATURE_COMMANDS(name) static mib_command_handler MIB_FEATURE_CMD_MAP(name) [] = 
#define MIB_FEATURE_CMD_COUNT(name) (sizeof(MIB_FEATURE_CMD_MAP(name))/sizeof(mib_command_handler))

#define DEFINE_MIB_FEATURE(name) feature_map MIB_FEATURE_OBJ(name) = { MIB_FEATURE_ID(name), MIB_FEATURE_CMD_COUNT(name), MIB_FEATURE_CMD_MAP(name) };
								 //void dummy_mib_##name##_importer() { feature_map* a = &MIB_FEATURE_OBJ(name); }
#define IMPORT_MIB_FEATURE(name) extern feature_map MIB_FEATURE_OBJ(name)

#define DEFINE_FEATURE_MAP() static const feature_map* the_mib_features[] = 
#define REGISTER_FEATURE_MAP() register_mib_features(the_mib_features, sizeof(the_mib_features)/sizeof(feature_map*) )

void register_mib_features( const feature_map** features, unsigned int count );

#endif