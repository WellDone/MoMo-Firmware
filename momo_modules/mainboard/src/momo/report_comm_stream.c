#include "report_comm_stream.h"
#include "momo_config.h"
#include "module_manager.h"
#include "scheduler.h"
#include "bus_master.h"
#include "system_log.h"
#include "utilities.h"
#include <string.h>

#define CONFIG current_momo_state.report_config

static char* report_buffer;
static uint8 report_stream_offset;
ModuleIterator comm_module_iterator;
bool current_stream_finished;

ScheduledTask retry_task;

void receive_gsm_stream_response(unsigned char a);
void report_rpc( MIBUnified *cmd, uint8 command, uint8 spec )
{
  cmd->address = module_iter_address( &comm_module_iterator );
  cmd->bus_command.feature = 11;
  cmd->bus_command.command = command;
  cmd->bus_command.param_spec = spec;
  bus_master_rpc_async(receive_gsm_stream_response, cmd); //TODO: Handle failure to send
}
void reset_comm_stream()
{
  report_stream_offset = 0;
  current_stream_finished = false;
}
void open_stream()
{
  DEBUG_LOGL( "Opening comm stream..." );
  MIBUnified cmd;
  memcpy( cmd.mib_buffer, CONFIG.report_server_address, strlen(CONFIG.report_server_address) );
  DEBUG_LOG( cmd.mib_buffer, strlen(CONFIG.report_server_address) );
  report_rpc( &cmd, 0, plist_with_buffer(0,strlen(CONFIG.report_server_address)) );
}
void next_comm_module( void* arg )
{
  module_iter_next( &comm_module_iterator );
  reset_comm_stream();
  if ( module_iter_get( &comm_module_iterator ) != NULL )
  {
    open_stream();
  }
  else
  {
    DEBUG_LOGL( "Finished streaming report to all comm modules." );
  }
}

void stream_to_gsm() {
  MIBUnified cmd;
  if ( report_stream_offset >= strlen(report_buffer) )
  {
    DEBUG_LOGL( "Closing comm stream." );
    current_stream_finished = true;
    report_rpc( &cmd, 2, plist_empty() );
    return;
  }

  DEBUG_LOGL( "Streaming data..." );
  uint8 byte_count = strlen(report_buffer)-report_stream_offset;
  if ( byte_count > kBusMaxMessageSize )
    byte_count = kBusMaxMessageSize;
  memcpy( cmd.mib_buffer, report_buffer+report_stream_offset, byte_count );
  DEBUG_LOG( cmd.mib_buffer, byte_count );
  report_stream_offset += byte_count;
  report_rpc( &cmd, 1, plist_with_buffer( 0, byte_count ) );
}
void receive_gsm_stream_response(unsigned char a) 
{
  FLUSH_LOG();
  if ( a != kNoMIBError ) {
    CRITICAL_LOGL( "Failed to send a message to a comm module!  Error: " );
    char buf[4];
    CRITICAL_LOG( buf, itoa_small( buf, 4, a ) );

    notify_report_failure();
  }
  else if ( !current_stream_finished )
  {
    taskloop_add( stream_to_gsm, NULL );
  }
  // else wait for success/failure notification
  FLUSH_LOG();
}

void report_stream_abandon()
{
  scheduler_remove_task( &retry_task );
  //TODO: Wait for the current stream (if any) to be closed.
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
  DEBUG_LOGL( "Report succeeded." );
  taskloop_add( next_comm_module, NULL );
}
void notify_report_failure()
{
  DEBUG_LOGL( "Report failed.  Retrying." );
  // TODO: Save success or failure to the report log.
  scheduler_schedule_task( post_report, CONFIG.report_interval - 1, 1, &retry_task, NULL ); // if we're reporting every day, retry every hour
  // TODO: This blocks streaming to any other module, which could be problematic
}