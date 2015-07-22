#include "report_comm_stream.h"
#include "momo_config.h"
#include "module_manager.h"
#include "scheduler.h"
#include "bus_master.h"
#include "system_log.h"
#include "utilities.h"
#include <string.h>
#include "log_definitions.h"

#define CONFIG current_momo_state.report_config

static char*    report_buffer;
static char*    report_route;
static uint8    report_route_counter;
static uint8    report_stream_offset;
static uint8_t  comm_module_address;

bool current_stream_finished;
bool comm_module_connected;
uint8 retry_count = 0;

CommStreamState comm_info;

ScheduledTask report_retry_task;

void receive_gsm_stream_response(unsigned char a);

static void report_transition_states(ReportStreamingState new_state);
static void report_streaming_state_machine(void *arg);
static void set_comm_destination(unsigned char a);
static void apn_is_set(uint8_t result);
static void set_apn();

void init_comm_stream()
{
  report_retry_task.flags = 0; //Make sure we initialize this to avoid corrupting the scheduler

  comm_info.address = find_module_by_name("gsmcom");
  if (comm_info.address == 0)
    comm_info.streaming_allowed = false;
  else
    comm_info.streaming_allowed = true;

  comm_info.last_error = 0;
  comm_info.state = kIdleState;

  report_route = CONFIG.route_primary;
}

static void report_streaming_state_machine(void *arg)
{
  LOG_DEBUG(kReportStreamStateExecuted);
  LOG_INT(comm_info.state);

  switch (comm_info.state)
  {
    case kIdleState:
    break;

    case kStartStreamingState:
    //TODO: check for existing streams ongoing
    report_transition_states(kSetDestination);
    break;

    case kSetDestination:
    report_route_counter = 0;
    set_comm_destination(0);
    break;

    case kSetAPN:
    set_apn();
    break;
  }
}

void report_fatal_error(int code)
{
  comm_info.last_error = code;
  report_transition_states(kErrorOccurredState);
}

static void report_transition_states(ReportStreamingState new_state)
{
  comm_info.state = new_state;
  taskloop_add(report_streaming_state_machine, NULL);
}

void report_start_streaming()
{
  //FIXME: finish this function
}

void report_rpc( MIBUnified *cmd, uint8 command, uint8 len )
{
  cmd->address = comm_module_address;
  cmd->packet.call.command = pack_command(11, command);
  cmd->packet.call.length = len;

  bus_master_rpc_async(receive_gsm_stream_response, cmd);
}

void reset_comm_stream()
{
  report_stream_offset = 0;
  current_stream_finished = false;
}

void open_stream()
{
  ++CONFIG.transmit_sequence;
  
  MIBUnified cmd;
  *((uint16*)cmd.packet.data) = strlen(report_buffer);
  
  LOG_DEBUG(kOpenedCommStreamNotice);
  LOG_INT(comm_info.address);
  LOG_INT(*((uint16*)cmd.packet.data));

  report_rpc( &cmd, 0, 2);
}

static void set_comm_destination(unsigned char a)
{
  if ( report_route_counter >= strlen(report_route) )
  {
    report_transition_states(kSetAPN);
    return;
  }

  MIBUnified cmd;

  uint8 len = kMIBBufferSize - 2; // Leave room for the start integer
  if ( (strlen(report_route) - report_route_counter) < len )
    len = strlen(report_route) - report_route_counter;

  *((uint16*)cmd.packet.data) = report_route_counter;

  LOG_DEBUG(kSetCommDestinationNotice);
  LOG_INT(report_route_counter);
  LOG_INT(len);
  LOG_STRING(report_route+report_route_counter);

  memcpy( cmd.packet.data+2, report_route+report_route_counter, len );

  report_route_counter += len;

  cmd.address = comm_info.address;
  cmd.packet.call.command = pack_command(11, 4);
  cmd.packet.call.length = 2 + len;
  
  bus_master_rpc_async(set_comm_destination, &cmd);
}

static void set_apn() //TODO: Support non-GSM comm boards
{
  LOG_DEBUG(kSetAPNNotice);
  LOG_STRING(CONFIG.gprs_apn);

  MIBUnified cmd;
  cmd.address = comm_info.address;
  cmd.packet.call.command = pack_command(10, 9);
  cmd.packet.call.length = strlen(CONFIG.gprs_apn);

  memcpy(cmd.packet.data, CONFIG.gprs_apn, strlen(CONFIG.gprs_apn));
  
  bus_master_rpc_async(apn_is_set, &cmd);
}

static void apn_is_set(uint8_t result)
{
  report_transition_states(kIdleState);
}

void stream_to_gsm() {
  MIBUnified cmd;
  if ( report_stream_offset >= strlen(report_buffer) )
  {
    LOG_DEBUG(kClosingCommStreamNotice);
    current_stream_finished = true;
    report_rpc( &cmd, 2, 0);
    return;
  }

  LOG_DEBUG(kStreamingCommDataNotice);
  uint8 byte_count = strlen(report_buffer)-report_stream_offset;

  if ( byte_count > kMIBBufferSize )
    byte_count = kMIBBufferSize;

  memcpy( cmd.packet.data, report_buffer+report_stream_offset, byte_count );

  LOG_ARRAY(cmd.packet.data, byte_count);
  report_stream_offset += byte_count;
  report_rpc( &cmd, 1, byte_count);
}

void receive_gsm_stream_response(unsigned char a) 
{
  LOG_FLUSH();

  if (a != kNoErrorStatus)
  {
    LOG_CRITICAL(kFailedMessageToCommModuleNotice);
    LOG_INT(a);
    notify_report_failure();
  }
  else if ( !current_stream_finished )
  {
    taskloop_add( stream_to_gsm, NULL );
  }
  // else wait for success/failure notification
  LOG_FLUSH();
}

void report_stream_abandon()
{
  //TODO: finish this function
  scheduler_remove_task(&report_retry_task);
}

int report_stream_send(char* buffer)
{
  if (comm_info.state == kIdleState && comm_info.streaming_allowed)
  {
    report_buffer = buffer;
    report_transition_states(kStartStreamingState);
    return 1;
  }

  return 0;
}

void notify_report_success()
{
  // TODO: Save success or failure to the report log.
  LOG_DEBUG(kReportSucceededNotice);

  //TODO: finish this function
  //taskloop_add(next_comm_module, NULL);
}

void notify_report_failure()
{
  /*if ( !comm_module_iterator.started )
    return; // Something else triggered a report, there's nothing to retry

  // TODO: Save success or failure to the report log.
  if ( retry_count < CONFIG.retry_limit )
  {
    int retry_interval = CONFIG.report_interval -1;
    if (retry_interval < 0)
      retry_interval = 0;

    ++retry_count;
    LOG_DEBUG(kReportFailedRetryNotice);
    LOG_INT(retry_count);
    reset_comm_stream();
    scheduler_schedule_task( open_stream, retry_interval, 1, &report_retry_task, NULL ); // if we're reporting every day, retry every hour
    // TODO: This blocks streaming to any other module, which could be problematic
  }
  else if ( report_route == CONFIG.route_primary && CONFIG.route_secondary[0] != '\0' )
  {
    LOG_DEBUG(kReportFailedRerouteNotice);
    LOG_STRING(CONFIG.route_secondary);
    retry_count = 0;
    report_route = CONFIG.route_secondary;
    reset_comm_stream();
    taskloop_add( open_stream, NULL );
  }
  else
  {
    LOG_DEBUG(kReportAbandonedNotice);
    taskloop_add( next_comm_module, NULL );
  }*/
}