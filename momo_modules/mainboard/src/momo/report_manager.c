#include "report_manager.h"
#include "scheduler.h"
#include "momo_config.h"
#include "sensor_event_log.h"
#include "utilities.h"
#include "uart.h"
#include "base64.h"
#include <stdlib.h>
#include <string.h>
#include "bus_master.h"
#include "module_manager.h"
#include "perf.h"
#include "system_log.h"
#include "report_log.h"

#define EVENT_BUFFER_SIZE         1

#define CONFIG current_momo_state.report_config

//SMS Report structure, 118 bytes total
typedef struct 
{ 
    //16 byte header
    uint8          report_version;         // = 2, must be positive
    uint8          sensor_id;              //   The unique ID of this sensor related to this controller.
    uint16         sequence;               //2 - which report # is this (for this sensor/stream?)
    uint16         flags;                  //   Various flags determining how the data will be aggregated and other things

    uint16         battery_voltage;        //2
    uint16         diagnostics[2];         //4  diagnostic flags and data, usage TBD

    uint8          bulk_aggregates;        //  The aggregate functions to run on the entire span
    uint8          interval_aggregates;    //  The aggregate functions to run on each interval

    union
    {
      struct
      {
      uint8        interval_type:4;        //  0: second, 1: minute, 2: hour, 3: day
      uint8        interval_step:4;        //
      };

      uint8        interval_union;
    };

    uint8          interval_count;         //  Up to 256 'intervals' each aggregated individually
    
    // 102 bytes for data (56 two byte report values)
    uint16         data[NUM_BUCKETS];
} sms_report;

typedef struct
{
  uint8 status;
  uint8 retry_count;
  ModuleIterator comm_module_iterator;
  uint8 stream_offset;
} sms_report_status;

typedef struct 
{
  uint16 count;
  uint16 min;
  uint16 max;
  uint16 sum;
} agg_counters;

static sms_report        current_report;
static sms_report_status current_report_status; 
uint8  report_map;
static ScheduledTask  report_task;
char base64_report_buffer[BASE64_REPORT_MAX_LENGTH+1];
static sensor_event   event_buffer[EVENT_BUFFER_SIZE];

//TODO: Implement dynamic report routing based on an initial "registration" ping to the coordinator address
static const char   default_server_gsm_address[16] = {'+','1','4','1','5','9','9','2','8','3','7','0','\0'};
extern unsigned int last_battery_voltage;

void report_manager_start()
{
  if ( get_momo_state_flag( kStateFlagReportingEnabled ) )
    start_report_scheduling();
}

void init_report_config()
{
  memcpy( CONFIG.report_server_address, default_server_gsm_address, 16 );

  CONFIG.current_sequence          = 0;
  CONFIG.report_interval           = kEveryDay;
  CONFIG.report_flags              = kReportFlagDefault;
  CONFIG.bulk_aggregates           = kAggCount | kAggMin | kAggMax;
  CONFIG.interval_aggregates       = kAggCount | kAggMean;

  //Make sure we initialize this scheduled task.
  report_task.flags = 0;
}

void update_interval_headers( sms_report* report, AlarmRepeatTime interval )
{
  switch ( interval )
  {
    case kEvery10Seconds:
      report->interval_union = pack_report_interval(kReportIntervalSecond, 1);
      report->interval_count = 10;
      break;
    case kEveryMinute:
      report->interval_union = pack_report_interval(kReportIntervalSecond, 6);
      report->interval_count = 10;
      break;
    case kEvery10Minutes:
      report->interval_union = pack_report_interval(kReportIntervalMinute, 1);
      report->interval_count = 10;
      break;
    case kEveryHour:
      report->interval_union = pack_report_interval(kReportIntervalMinute, 6);
      report->interval_count = 10;
      break;
    case kEveryDay:
    default:
      report->interval_union = pack_report_interval(kReportIntervalHour, 1);
      report->interval_count = 24;
      break;
  }
}

uint32 create_time_delta( const sms_report* report )
{
  //FIXME: is 0 a valid option for the caller of this function?
  switch ( report->interval_type )
  {
    case kReportIntervalSecond:
      return report->interval_step * report->interval_count;
    case kReportIntervalMinute:
      return 60 * report->interval_step * report->interval_count;
    case kReportIntervalHour:
      return 3600L * report->interval_step * report->interval_count;
  }
  return 0;
}

//Set bit counting routine from
//https://graphics.stanford.edu/~seander/bithacks.html
static uint8 agg_size(uint8 agg_set)
{
  uint8 size;

  for (size=0; agg_set; ++size)
    agg_set &= agg_set-1;

  return size;
}

static void init_agg( agg_counters* agg )
{
  agg->count = 0;
  agg->min = MAX_UINT16;
  agg->max = 0;
  agg->sum = 0;
}
static void update_agg( agg_counters* agg, sensor_event* event )
{
  ++(agg->count);
  agg->sum += event->value;
  if ( event->value < agg->min )
    agg->min = event->value;
  if ( event->value > agg->max )
    agg->max = event->value;
}
static uint8 finish_agg( agg_counters* agg, sms_report* report, uint8 agg_set, uint8 index)
{
  if ( agg_set & kAggCount )
    report->data[index++] = agg->count;
  if ( agg_set & kAggSum )
    report->data[index++] = agg->sum;
  if ( agg_set & kAggMean )
    report->data[index++] = (agg->count==0)? 0 : agg->sum / agg->count;
  if ( agg_set & kAggMin )
    report->data[index++] = agg->min;
  if ( agg_set & kAggMax )
    report->data[index++] = agg->max;

  return index;
}

bool construct_report()
{
  PROFILE_START(kConstructReport);
  agg_counters    bulk_agg;
  agg_counters    int_agg;
  rtcc_timestamp  now;
  uint8           i, c;
  uint8           curr_interval = 0;
  uint8           interval_bucket = agg_size(CONFIG.bulk_aggregates);
  uint8           max_bucket = NUM_BUCKETS - agg_size(CONFIG.interval_aggregates);

  sms_report* report = &current_report;
  current_report_status.status = kReportStatusNone;

  //Initialize the report header
  report->report_version = 2;
  report->sensor_id = 0;
  report->sequence = CONFIG.current_sequence++;
  report->flags = CONFIG.report_flags;
  report->battery_voltage = last_battery_voltage;
  report->diagnostics[0] = sensor_event_log_count();
  report->diagnostics[1] = 0;
  report->bulk_aggregates = CONFIG.bulk_aggregates;
  report->interval_aggregates = CONFIG.interval_aggregates;

  update_interval_headers(&report, CONFIG.report_interval);

  uint32 time_delta = create_time_delta(report);
  uint32 time_step = time_delta / report->interval_count;

  init_agg(&bulk_agg);
  init_agg(&int_agg);
  now = rtcc_get_timestamp();

  //Zero out the report structure
  for (i=0; i<NUM_BUCKETS; ++i)
    report->data[i] = 0;

  do
  {
    PROFILE_START(kProcessEventCounter);
    c = read_sensor_events( event_buffer, EVENT_BUFFER_SIZE );
    for ( i = 0; i < c; ++i )
    {
      if ( report->sensor_id == 0 )
        report->sensor_id = event_buffer[i].module;
      else if ( report->sensor_id != event_buffer[i].module )
        continue; // TODO: Support multiple sensor streams

      if (CONFIG.bulk_aggregates != kAggNone)
        update_agg( &bulk_agg, &event_buffer[i] );

      if (CONFIG.interval_aggregates != kAggNone)
      {
        uint32 time_seconds;
        uint8  event_interval;
        TimeIntervalDirection delta;

        if (interval_bucket >= max_bucket)
          continue; //TODO: Extend to a second SMS

        //Check if the event is too old and drop it
        //TODO: extend the report start backwards to pick up the dropped events?
        time_seconds = rtcc_timestamp_difference(event_buffer[i].timestamp, now, &delta);
        if (time_seconds >= time_delta)
          continue;

        //Check if this event is somehow in the future
        if (delta == kNegativeDelta)
        {
          requeue_sensor_events( c - i );
          c = 0;
          break;
        }

        event_interval = report->interval_count - time_seconds/time_step - 1; //time_seconds/time_step < report->interval_count since time_seconds < time_delta

        //Check if this event corresponds to a new interval, in which case zero out the intervening
        //intervals.
        while (curr_interval < event_interval)
        {
          interval_bucket = finish_agg(&int_agg, report, CONFIG.interval_aggregates, interval_bucket);
          init_agg(&int_agg);

          ++curr_interval;
        }

        update_agg(&int_agg, &event_buffer[i]);
      }
    }
    PROFILE_END(kProcessEventCounter);
  }
  while (c > 0);

  //Finish the last interval
  if (CONFIG.interval_aggregates != kAggNone && interval_bucket < max_bucket)
    interval_bucket = finish_agg(&int_agg, report, CONFIG.interval_aggregates, interval_bucket);

  if ( CONFIG.bulk_aggregates != kAggNone)
    finish_agg(&bulk_agg, report, CONFIG.bulk_aggregates, 0);

  save_raw_report( report );

  PROFILE_START(kEncodeReportCounter);
  i = base64_encode((const BYTE*)report, RAW_REPORT_MAX_LENGTH, base64_report_buffer, BASE64_REPORT_MAX_LENGTH);
  PROFILE_END(kEncodeReportCounter);
  base64_report_buffer[i] = '\0';
  
  current_report_status.status = kReportStatusConstructed;
  PROFILE_END(kConstructReport);
  return true;
}

void receive_gsm_stream_response(unsigned char a);
void report_rpc( MIBUnified *cmd, uint8 command, uint8 spec )
{
  cmd->address = module_iter_address( &current_report_status.comm_module_iterator );
  cmd->bus_command.feature = 11;
  cmd->bus_command.command = command;
  cmd->bus_command.param_spec = spec;
  bus_master_rpc_async(receive_gsm_stream_response, cmd); //TODO: Handle failure to send
}
void next_comm_module( void* arg )
{
  module_iter_next( &current_report_status.comm_module_iterator );
  current_report_status.stream_offset = 0;
  if ( module_iter_get( &current_report_status.comm_module_iterator ) != NULL )
  {
    current_report_status.status = kReportStatusOpeningStream;
    DEBUG_LOGL( "Opening comm stream..." );
    MIBUnified cmd;
    memcpy( cmd.mib_buffer, CONFIG.report_server_address, strlen(CONFIG.report_server_address) );
    DEBUG_LOG( cmd.mib_buffer, strlen(CONFIG.report_server_address) );
    report_rpc( &cmd, 0, plist_with_buffer(0,strlen(CONFIG.report_server_address)) );
  }
  else
  {
    current_report_status.status = kReportStatusPending;
  }
}

void stream_to_gsm() {
  MIBUnified cmd;
  if ( current_report_status.stream_offset >= strlen(base64_report_buffer) )
  {
    DEBUG_LOGL( "Closing comm stream." );
    current_report_status.status = kReportStatusClosingModule;
    report_rpc( &cmd, 2, plist_empty() );
    return;
  }

  current_report_status.status = kReportStatusStreaming;
  DEBUG_LOGL( "Streaming data..." );
  uint8 byte_count = strlen(base64_report_buffer)-current_report_status.stream_offset;
  if ( byte_count > kBusMaxMessageSize )
    byte_count = kBusMaxMessageSize;
  memcpy( cmd.mib_buffer, base64_report_buffer+current_report_status.stream_offset, byte_count );
  DEBUG_LOG( cmd.mib_buffer, byte_count );
  current_report_status.stream_offset += byte_count;
  report_rpc( &cmd, 1, plist_with_buffer( 0, byte_count ) );
  save_momo_state();
}
void receive_gsm_stream_response(unsigned char a) 
{
  FLUSH_LOG();
  if ( a != kNoMIBError || current_report_status.status == kReportStatusClosingModule ) {
    if ( a != kNoMIBError )
    {
      CRITICAL_LOGL( "Failed to send a message to a comm module!  Error: " );
      char buf[4];
      CRITICAL_LOG( buf, itoa_small( buf, 4, a ) );
    }
    taskloop_add( next_comm_module, NULL );
  }
  else
  {
    taskloop_add( stream_to_gsm, NULL );
  }
  FLUSH_LOG();
}

void post_report( void* arg ) 
{
  DEBUG_LOGL( "Constructing report..." );
  if (!construct_report( CONFIG.report_interval ))
  {
    CRITICAL_LOGL( "Failed to construct report!" );
    return; //TODO: Recover
  }

  current_report_status.comm_module_iterator = create_module_iterator( kMIBCommunicationType );
  taskloop_add( next_comm_module, NULL );
}

void start_report_scheduling() {
  DEBUG_LOGL( "Report scheduling started." );
  scheduler_schedule_task( post_report, CONFIG.report_interval, kScheduleForever, &report_task, NULL );
}
void stop_report_scheduling() {
  DEBUG_LOGL( "Report scheduling stopped." );
  scheduler_remove_task( &report_task );
}
void set_report_scheduling_interval( AlarmRepeatTime interval ) {
  if ( interval >= kNumAlarmTimes )
    return;
  CONFIG.report_interval = interval;
  if ( BIT_TEST(report_task.flags, kBeingScheduledBit) )
    scheduler_schedule_task( post_report, CONFIG.report_interval, kScheduleForever, &report_task, NULL );
  save_momo_state();
}
void set_report_server_address( const char* address, uint8 len )
{
  if ( len >= sizeof(CONFIG.report_server_address) )
    return;
  memcpy( CONFIG.report_server_address, address, len );
  CONFIG.report_server_address[len] = '\0';
  save_momo_state();
}