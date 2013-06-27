#ifndef __mib_command_h__
#define __mib_command_h__

#include "protocol.h"
#include "common_types.h"

enum
{
	kMIBCreateParameters = 1,
	kMIBExecuteCommand = 2
};

typedef enum
{
	kMIBTestFeature = 2,
	kMIBProgrammingFeature = 255
} MIBFeature;

typedef struct {
	const BYTE           command_tag;
	const mib_callback   handler;
	const BYTE           param_spec;
} mib_command_handler;

typedef struct {
	const MIBFeature     feature;
	mib_command_handler* commands;
	unsigned int         command_count;
} feature_map;

#define mk_feature_map(feature,commands) {feature, commands, (sizeof(commands)/sizeof(mib_command_handler))}

mib_command_handler*	find_handler(MIBFeature feature, BYTE cmd);
void register_mib_features( const feature_map* features, unsigned int count );

#endif