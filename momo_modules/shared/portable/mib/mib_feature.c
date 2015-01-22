#include "mib_feature.h"

const feature_map** the_features;
unsigned int the_feature_count;

void register_mib_features( const feature_map** features, unsigned int count )
{
	the_features = features;
	the_feature_count = count;
}