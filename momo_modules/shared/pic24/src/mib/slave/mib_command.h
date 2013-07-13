#ifndef __mib_command_h__
#define __mib_command_h__

#include "protocol.h"
#include "common_types.h"

enum
{
	kMIBCreateParameters = 1,
	kMIBExecuteCommand = 2
};

typedef struct {
	const BYTE           command_tag;
	const mib_callback   handler;
	const BYTE           param_spec;
} mib_command_handler;

typedef struct {
	unsigned int         id;
	mib_command_handler* commands;
	unsigned int         command_count;
} feature_map;

mib_command_handler*	find_handler(unsigned int feature_id, BYTE cmd);
void register_mib_features( const feature_map** features, unsigned int count );

#endif