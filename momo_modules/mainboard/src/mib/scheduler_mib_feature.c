#include "bus_slave.h"
#include "mib_definitions.h"
#include "mib_feature_definition.h"
#include "scheduler_mib_feature.h"
#include "scheduler.h"
#include "common_types.h"
#include "bus_master.h"
#include "system_log.h"

typedef struct {
    uint8 address;
    uint8 feature;
    uint8 command;

    AlarmRepeatTime frequency;

    ScheduledTask task;
    uint8 rpc_id;
    uint8 flags;
} RPCCallback;

static uint8 next_queue_rpc_id = 0;
static uint8 next_dequeue_rpc_id = 0;

#define MAX_SCHEDULED_CALLBACKS 16
static RPCCallback callbacks[MAX_SCHEDULED_CALLBACKS];

static void rpc_done( uint8 status )
{
    uint8 index;
    for ( index = 0; index < MAX_SCHEDULED_CALLBACKS; ++index )
    {
        if ( BIT_TEST(callbacks[index].flags, kRPCInProgress) && callbacks[index].rpc_id == next_dequeue_rpc_id )
        {
            CLEAR_BIT(callbacks[index].flags, kRPCInProgress);
            ++next_dequeue_rpc_id;
            if ( next_dequeue_rpc_id == 0 )
                next_dequeue_rpc_id = 1;
            return;
        }
    }
}
static void callback( void* arg )
{
    RPCCallback *cb = (RPCCallback*) arg;

    if (BIT(cb->flags, kRPCInProgress))
    {
        return;
    }

    MIBUnified cmd;
    cmd.address = cb->address;
    cmd.packet.call.command = (cb->feature << 8) | cb->command;
    cmd.packet.call.length = 0;

    SET_BIT(cb->flags, kRPCInProgress);
    cb->rpc_id = next_queue_rpc_id++;
    if ( next_queue_rpc_id == 0 )
        next_queue_rpc_id = 1;

    bus_master_rpc_async( rpc_done, &cmd );
}

uint8_t schedule_callback(uint8_t length)
{
    uint8 address = plist_get_int16(0) & 0xFF;
    uint8 feature = plist_get_int16(1) >> 8;
    uint8 command = plist_get_int16(1) & 0xFF;
    uint8 frequency = plist_get_int16(2) & 0xFF;

    uint8 index = 0;
    for ( index = 0; index < MAX_SCHEDULED_CALLBACKS; ++index )
    {
        if ( callbacks[index].address == 0 )
        {
            RPCCallback* cb = &(callbacks[index]);
            cb->address = address;
            cb->feature = feature;
            cb->command = command;
            cb->frequency = frequency;
            cb->flags = 0;
            cb->task.flags = 0; //Make sure we zero out flags so that the scheduler knows that this isn't an already scheduled task

            scheduler_schedule_task( callback, cb->frequency, kScheduleForever, &(cb->task), (void*) cb );
            break;
        }
    }
    if ( index == 16 )
        return kCallbackError;

    return kNoErrorStatus;
}

uint8_t stop_scheduled_callback(uint8_t length)
{
    uint8 address = plist_get_int16(0) & 0xFF;
    uint8 feature = plist_get_int16(1) >> 8;
    uint8 command = plist_get_int16(1) & 0xFF;
    uint8 frequency = plist_get_int16(2) & 0xFF;

    uint8 index;
    for ( index = 0; index < MAX_SCHEDULED_CALLBACKS; ++index )
    {
        if ( callbacks[index].address == address &&
               callbacks[index].feature == feature &&
               callbacks[index].command == command &&
               callbacks[index].frequency == frequency )
        {
            scheduler_remove_task( &callbacks[index].task );
            callbacks[index].address = 0;
        }
    }

    return kNoErrorStatus;
}

uint8_t get_callback_map(uint8_t length)
{
    uint16 callback_map = 0;

    uint8 index;
    for ( index = 0; index < MAX_SCHEDULED_CALLBACKS; ++index )
    {
        if ( callbacks[index].address != 0 )
            callback_map |= 0x1 << index;
    }
    bus_slave_return_int16( callback_map );

    return kNoErrorStatus;
}

uint8_t describe_callback(uint8_t length)
{
    uint8 index = plist_get_int8(0);
    if ( index >= MAX_SCHEDULED_CALLBACKS || callbacks[index].address == 0 )
        return kCallbackError;

    bus_slave_return_buffer( &(callbacks[index]), sizeof(RPCCallback) );

    return kNoErrorStatus;
}

DEFINE_MIB_FEATURE_COMMANDS(scheduler) {
    {0x00, schedule_callback},
    {0x01, stop_scheduled_callback},
    {0x02, get_callback_map},
    {0x03, describe_callback}
};
DEFINE_MIB_FEATURE(scheduler);