//beacon.h
#ifndef __beacon_h__
#define __beacon_h__

#include "rn4020.h"
#include "scheduler.h"

typedef struct
{
	unsigned int 	beacon_interval;
	unsigned int 	addr;
	unsigned int 	rpc;
	unsigned int 	sequence;
	ScheduledTask	task;
} beacon_data;

void beacon_init(AlarmRepeatTime sample_interval, unsigned int beacon_interval, unsigned int addr, unsigned int rpc);
void beacon_callback(void *data);
void beacon_update(uint8_t status, void *state);

#endif