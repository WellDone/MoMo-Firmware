#include "bus_master.h"
#include "mib_feature_definition.h"
#include "mib_state.h"

uint8_t finish_async_rpc(uint8_t length)
{
	bus_master_finish_async_rpc(mib_unified.packet.call.sender);

	bus_slave_set_returnbuffer_length(0);
	return 0;
}

DEFINE_MIB_FEATURE_COMMANDS(async)
{
	{0, finish_async_rpc}
};

DEFINE_MIB_FEATURE(async);