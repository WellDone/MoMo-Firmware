#include "beacon.h"
#include "bus_master.h"
#include <string.h>

beacon_data bc_state;

void beacon_init(AlarmRepeatTime sample_interval, unsigned int beacon_interval, unsigned int addr, unsigned int rpc)
{
	bc_state.beacon_interval = beacon_interval;
	bc_state.addr = addr;
	bc_state.rpc = rpc;
	bc_state.sequence = 0;

	bc_state.task.flags = 0;

	scheduler_schedule_task(beacon_callback, sample_interval, kScheduleForever, &bc_state.task, NULL);
}
void beacon_callback(void *data)
{
	MIBUnified cmd;
	cmd.address = bc_state.addr;
	cmd.packet.call.command = bc_state.rpc;
	cmd.packet.call.length = 0;
	bus_master_rpc_async(beacon_update, &cmd);
}

void beacon_update(uint8_t status)
{
	unsigned char update[23];

	update[0] = bc_state.sequence++;
	update[1] = status;
	update[2] = mib_unified.packet.response.length;
	memcpy(&update[3], mib_unified.packet.data, update[2]);

	bt_stop_advertising();
	bt_broadcast((char *)update, 23);
	bt_advertise(bc_state.beacon_interval, 0);
}