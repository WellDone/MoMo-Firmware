#include "mib_feature_definition.h"
#include "config_manager.h"
#include "bus_slave.h"

static uint8_t count_entries(uint8_t length)
{
	plist_set_int16(0, cm_num_entries());

	bus_slave_set_returnbuffer_length(2);
	return kNoErrorStatus;
}

DEFINE_MIB_FEATURE_COMMANDS(config) 
{
	{0x00, count_entries}
};

DEFINE_MIB_FEATURE(config);