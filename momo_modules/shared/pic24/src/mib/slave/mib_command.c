//mib_command.c 

#include "mib_command.h"
#include "bus_slave.h"
#include <string.h>
	
static const feature_map** the_features = NULL;
static unsigned int the_feature_count = 4;
//static feature_map* features;

extern volatile unsigned char 	mib_buffer[kBusMaxMessageSize];

mib_command_handler* find_handler(unsigned int feature_id, BYTE cmd)
{
	unsigned int i;
	const feature_map* found_feat = NULL;

	if (!the_features)
		return NULL;
	
	for (i=0; i<4; ++i)
	{
		if (the_features[i]->id == feature_id)
		{
			found_feat = the_features[i];
			break;
		}
	}

	if (found_feat == NULL)
		return NULL;

	for (i=0;i<found_feat->command_count;++i)
	{
		if ( cmd == found_feat->commands[i].command_tag ) {
			return &found_feat->commands[i];
		}
	}
	return NULL;
}

void register_mib_features( const feature_map** features, unsigned int count )
{
	the_features = features;
	the_feature_count = count;
}