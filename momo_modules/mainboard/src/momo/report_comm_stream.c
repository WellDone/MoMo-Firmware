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

static char* report_buffer;
static char* report_route;
static uint8 report_route_counter;
static uint8 report_stream_offset;
ModuleIterator comm_module_iterator;
bool current_stream_finished;
uint8 retry_count = 0;

ScheduledTask report_retry_task;

void receive_gsm_stream_response(unsigned char a);

void init_comm_stream()
{
  report_retry_task.flags = 0; //Make sure we initialize this to avoid corrupting the scheduler

  comm_module_iterator = create_module_iterator( kMIBCommunicationType );
}

void report_rpc( MIBUnified *cmd, uint8 command, uint8 spec )
{
  cmd->address = module_iter_address( &comm_module_iterator );
  cmd->bus_command.feature = 11;
  cmd->bus_command.command = command;
  cmd->bus_command.param_spec = spec;
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
  *((uint16*)cmd.mib_buffer) = strlen(report_buffer);
  
  LOG_DEBUG(kOpenedCommStreamNotice);
  LOG_INT(module_iter_address(&comm_module_iterator));
  LOG_INT(*((uint16*)cmd.mib_buffer));

  report_rpc( &cmd, 0, plist_ints(1) );
}
void set_comm_destination(unsigned char a)
{
  if ( report_route_counter >= strlen(report_route) )
  {
    open_stream();
    return;
  }

  MIBUnified cmd;

  uint8 len = kBusMaxMessageSize - 2; // Leave room for the start integer
  if ( strlen(report_route) - report_route_counter < len )
    len = strlen(report_route) - report_route_counter;
  *((uint16*)cmd.mib_buffer) = report_route_counter;

  LOG_DEBUG(kSetCommDestinationNotice);
  LOG_INT(report_route_counter);
  LOG_INT(len);
  LOG_STRING(report_route+report_route_counter);

  memcpy( cmd.mib_buffer+2, report_route+report_route_counter, len );

  report_route_counter += len;

  cmd.address = module_iter_address( &comm_module_iterator );
  cmd.bus_command.feature = 11;
  cmd.bus_command.command = 4;
  cmd.bus_command.param_spec = plist_with_buffer(1,len);
  
  bus_master_rpc_async(set_comm_destination, &cmd);
}
void set_apn() //TODO: Support non-GSM comm boards
{
  report_route_counter = 0;

  LOG_DEBUG(kSetAPNNotice);
  LOG_STRING(CONFIG.gprs_apn);

  MIBUnified cmd;
  cmd.address = module_iter_address( &comm_module_iterator );
  cmd.bus_command.feature = 10;
  cmd.bus_command.command = 9;
  cmd.bus_command.param_spec = plist_with_buffer(0,strlen(CONFIG.gprs_apn));
  memcpy( cmd.mib_buffer, CONFIG.gprs_apn, strlen(CONFIG.gprs_apn) );
  
  bus_master_rpc_async(set_comm_destination, &cmd);
}

void next_comm_module( void* arg )
{
  module_iter_next( &comm_module_iterator );
  reset_comm_stream();
  retry_count = 0;
  report_route = CONFIG.route_primary;
  if ( module_iter_get( &comm_module_iterator ) != NULL )
  {
    set_apn();
  }
  else
    LOG_DEBUG(kFinishedReportStreamingNotice);
}

void stream_to_gsm() {
  MIBUnified cmd;
  if ( report_stream_offset >= strlen(report_buffer) )
  {
    LOG_DEBUG(kClosingCommStreamNotice);
    current_stream_finished = true;
    report_rpc( &cmd, 2, plist_empty() );
    return;
  }

  LOG_DEBUG(kStreamingCommDataNotice);
  uint8 byte_count = strlen(report_buffer)-report_stream_offset;
  if ( byte_count > kBusMaxMessageSize )
    byte_count = kBusMaxMessageSize;
  memcpy( cmd.mib_buffer, report_buffer+report_stream_offset, byte_count );

  LOG_ARRAY(cmd.mib_buffer, byte_count);
  report_stream_offset += byte_count;
  report_rpc( &cmd, 1, plist_with_buffer( 0, byte_count ) );
}
void receive_gsm_stream_response(unsigned char a) 
{
  LOG_FLUSH();
  if ( a != kNoMIBError )
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
  if ( module_iter_get( &comm_module_iterator ) != NULL )
  {
    LOG_DEBUG(kAbandoningCommStreamNotice);
    current_stream_finished = true;
    
    MIBUnified cmd;
    cmd.address = module_iter_address( &comm_module_iterator );
    cmd.bus_command.feature = 11;
    cmd.bus_command.command = 3;
    cmd.bus_command.param_spec = plist_empty();
    bus_master_rpc_async(NULL, &cmd);

    while ( module_iter_get( &comm_module_iterator ) != NULL )
      module_iter_next( &comm_module_iterator );
  }

  scheduler_remove_task( &report_retry_task );
}
void report_stream_send( char* buffer )
{
  report_buffer = buffer;
  comm_module_iterator = create_module_iterator( kMIBCommunicationType );
  taskloop_add( next_comm_module, NULL ); 
}

void notify_report_success()
{
  // TODO: Save success or failure to the report log.
  LOG_DEBUG(kReportSucceededNotice);
  taskloop_add(next_comm_module, NULL);
}

void notify_report_failure()
{
  if ( !comm_module_iterator.started )
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
  }
}