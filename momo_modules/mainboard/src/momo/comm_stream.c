#include "comm_stream.h"
#include "momo_config.h"
#include "module_manager.h"
#include "scheduler.h"
#include "bus_master.h"
#include "system_log.h"
#include "utilities.h"
#include <string.h>
#include "log_definitions.h"

#define CONFIG current_momo_state.report_config

static void set_comm_destination(uint8_t rpc_status, void *arg);

static void apn_is_set(uint8_t result, void *arg);
static void set_apn(CommStreamState *info);

static void open_stream(CommStreamState *info);
static void stream_is_opened(unsigned char result, void *arg);

static void stream_data(CommStreamState *info);
static void continue_streaming_data(uint8_t rpc_status, void *arg);

static void close_stream(CommStreamState *info);
static void stream_is_closed(unsigned char result, void *arg);

static void commstream_transition_state(CommStreamState *info, ReportStreamingState new_state);
static void commstream_task(void *arg);
static void commstream_log_error(uint8_t address, CommStreamError error_code);
static void commstream_handle_error(CommStreamState *info, uint8_t error_code);

void commstream_init(CommStreamState *state)
{
    state->state = kCSIdleState;
    state->report = NULL;
    state->report_length = 0;
    state->scratch_counter = 0;
    state->last_error = 0;
    state->flags.value = 0;
    state->callback = NULL;

    state->address = kMIBInvalidAddress;
}

void commstream_setcallback(CommStreamState *info, task_callback callback)
{
    info->callback = callback;
}

void commstream_task(void *arg)
{
    CommStreamState *state = (CommStreamState *)arg;

    if (state == NULL)
    {
        commstream_log_error(kMIBInvalidAddress, kCSInvalidCommStreamStateObject);
        return;
    }

    LOG_DEBUG(kCommStreamStateExecuted);
    LOG_INT(state->address);
    LOG_INT(state->state);

    switch (state->state)
    {
        case kCSIdleState:
        break;

        case kCSStartStreamingState:
        commstream_transition_state(state, kCSSetDestination);
        break;

        case kCSSetDestination:
        state->scratch_counter = 0;
        set_comm_destination(kNoErrorStatus, arg);
        break;

        case kCSSetAPN:
        set_apn(state);
        break;

        case kCSOpenStream:
        open_stream(state);
        break;

        case kCSPushToStream:
        state->scratch_counter = 0;
        stream_data(state);
        break;

        case kCSCloseStream:
        close_stream(state);
        break;

        case kCSFinishStreamingState:
        commstream_transition_state(state, kCSIdleState);
        break;

        case kCSErrorOccurredState:
        commstream_transition_state(state, kCSIdleState);
        break;
    }
}

CommStreamError commstream_start(CommStreamState *state, uint8_t address, const char *report, unsigned int length)
{
    if (state == NULL)
    {
        commstream_log_error(kMIBInvalidAddress, kCSInvalidCommStreamStateObject);
        return kCSInvalidCommStreamStateObject;
    }

    if (state->state != kCSIdleState)
    {
        commstream_log_error(state->address, kCSStreamAlreadyInProgress);
        return kCSStreamAlreadyInProgress;
    }

    state->address = address;
    state->report = report;
    state->report_length = length;
    state->last_error = kCSNoError;

    commstream_transition_state(state, kCSStartStreamingState);
    return kCSNoError;
}

static void commstream_transition_state(CommStreamState *info, ReportStreamingState new_state)
{
  info->state = new_state;
  taskloop_add(commstream_task, info);
}

static void commstream_log_error(uint8_t address, CommStreamError error_code)
{
    LOG_DEBUG(kCommStreamFatalError);
    LOG_INT(address);
    LOG_INT(error_code);
}

static void commstream_handle_error(CommStreamState *info, uint8_t error_code)
{
    commstream_log_error(info->address, error_code);
    info->last_error = error_code;

    commstream_transition_state(info, kCSErrorOccurredState);
}

static void set_comm_destination(uint8_t rpc_status, void *arg)
{
    CommStreamState *info = (CommStreamState *)arg;

    unsigned int route_length = strlen(CONFIG.route_primary);
    if (info->scratch_counter >= route_length)
    {
        commstream_transition_state(info, kCSSetAPN);
        return;
    }

    MIBUnified cmd;

    uint8_t len = kMIBBufferSize - 2; // Leave room for the start integer
    
    if ((route_length - info->scratch_counter) < len)
        len = route_length - info->scratch_counter;

    *((uint16*)cmd.packet.data) = info->scratch_counter;

    memcpy(cmd.packet.data+2, CONFIG.route_primary + info->scratch_counter, len);
    LOG_DEBUG(kCommStreamSettingDestination);
    LOG_INT(info->address);
    LOG_ARRAY(CONFIG.route_primary + info->scratch_counter, len);
    LOG_INT(info->scratch_counter);
    LOG_INT(len);

    info->scratch_counter += len;

    cmd.address = info->address;
    cmd.packet.call.command = pack_command(11, 4);
    cmd.packet.call.length = 2 + len;

    bus_master_rpc_async(set_comm_destination, &cmd, arg);
}

static void stream_data(CommStreamState *info)
{
    MIBUnified  cmd;
    uint8_t     len = kMIBBufferSize;
    
    if ((info->report_length - info->scratch_counter) < len)
        len = info->report_length - info->scratch_counter;

    memcpy(cmd.packet.data, info->report + info->scratch_counter, len);

    info->scratch_counter += len;

    cmd.address = info->address;
    cmd.packet.call.command = pack_command(11, 1);
    cmd.packet.call.length = len;

    LOG_DEBUG(kCommStreamSendingData);
    LOG_INT(info->address);
    LOG_ARRAY(cmd.packet.data, len);
    LOG_INT(len);

    bus_master_rpc_async(continue_streaming_data, &cmd, info);
}

static void continue_streaming_data(uint8_t rpc_status, void *arg)
{
    CommStreamState *info = (CommStreamState *)arg;

    if (rpc_status != kNoErrorWithDataStatus)
    {
        commstream_handle_error(info, rpc_status);
        return;
    }
    else if (plist_get_int16(0) != 0)
    {
        commstream_handle_error(info, plist_get_int16(0));
        return;
    }
    else if (info->scratch_counter >= info->report_length)
    {
        commstream_transition_state(info, kCSCloseStream);
        return;
    }

    //Otherwise we've successfully streamed some data but there is more to go
    stream_data(info);
}

static void set_apn(CommStreamState *info)
{
    MIBUnified cmd;
    cmd.address = info->address;
    cmd.packet.call.command = pack_command(10, 9);
    cmd.packet.call.length = strlen(CONFIG.gprs_apn);

    LOG_DEBUG(kCommStreamSettingAPN);
    LOG_INT(info->address);

    memcpy(cmd.packet.data, CONFIG.gprs_apn, strlen(CONFIG.gprs_apn));
    LOG_ARRAY(cmd.packet.data, strlen(CONFIG.gprs_apn));

    bus_master_rpc_async(apn_is_set, &cmd, info);
}

static void apn_is_set(uint8_t result, void *arg)
{
    CommStreamState *info = (CommStreamState *)arg;
    commstream_transition_state(info, kCSOpenStream);
}

static void open_stream(CommStreamState *info)
{
    MIBUnified cmd;
    cmd.address = info->address;
    cmd.packet.call.command = pack_command(11, 0);
    cmd.packet.call.length = 2;

    cmd.packet.data[0] = info->report_length;
    cmd.packet.data[1] = 0;

    bus_master_rpc_async(stream_is_opened, &cmd, info);
}

static void stream_is_opened(unsigned char result, void *arg)
{
    CommStreamState *info = (CommStreamState*)arg;

    if (result == kNoErrorWithDataStatus && (plist_get_int16(0) == 0))
        commstream_transition_state(info, kCSPushToStream);
    else
    {
        if (result != kNoErrorWithDataStatus)
            commstream_handle_error(info, result);
        else
            commstream_handle_error(info, plist_get_int16(0));
    }
}

static void close_stream(CommStreamState *info)
{
    MIBUnified cmd;
    cmd.address = info->address;
    cmd.packet.call.command = pack_command(11, 2);
    cmd.packet.call.length = 0;

    bus_master_rpc_async(stream_is_closed, &cmd, info);
}

static void stream_is_closed(unsigned char result, void *arg)
{
    CommStreamState *info = (CommStreamState*)arg;

    if (result == kNoErrorWithDataStatus && (plist_get_int16(0) == 0))
        commstream_transition_state(info, kCSFinishStreamingState);
    else
    {
        if (result != kNoErrorWithDataStatus)
            commstream_handle_error(info, result);
        else
            commstream_handle_error(info, plist_get_int16(0));
    }
}

void report_stream_abandon()
{
  //TODO: finish this function
  //scheduler_remove_task(&report_retry_task);
}