#include "mainboard_mib_commands.h"
#include "mib_features.h"

DEFINE_FEATURE_MAP()
{
	MIB_FEATURE(test),
	MIB_FEATURE(controller),
	MIB_FEATURE(firmware_cache),
	MIB_FEATURE(reporting),
};

void init_mainboard_mib()
{
	REGISTER_FEATURE_MAP();
}
