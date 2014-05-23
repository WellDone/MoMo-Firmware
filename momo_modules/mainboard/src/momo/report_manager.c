#include "report_manager.h"
#include "common.h"
#include "scheduler.h"
#include "momo_config.h"
#include "sensor_event_log.h"
#include "utilities.h"
#include "uart.h"
#include "base64.h"
#include <stdlib.h>
#include <string.h>
#include "bus_master.h"

#define EVENT_BUFFER_SIZE 1
#define RAW_REPORT_MAX_LENGTH 118
#define BASE64_REPORT_MAX_LENGTH 160 //( 4 * ( ( RAW_REPORT_MAX_LENGTH + 2 ) / 3) )

static BYTE report_buffer[RAW_REPORT_MAX_LENGTH];
static char base64_report_buffer[BASE64_REPORT_MAX_LENGTH+1];

//TODO: Implement dynamic report routing based on an initial "registration" ping to the coordinator address
//static const char fallback_server_gsm_address[13] = {'+','1','4','1','5','9','9','2','8','3','7','0','\0'};

extern unsigned int last_battery_voltage;
static sensor_event event_buffer[EVENT_BUFFER_SIZE];

typedef struct { //16
    uint8          report_version;         // = 2, must be positive
    uint8          sensor_id;              //   The unique ID of this sensor related to this controller.
    uint16         sequence;               //2 - which report # is this (for this sensor/stream?)
    uint16         flags;                  //   Various flags determining how the data will be aggregated and other things

    uint16         battery_voltage;        //2
    uint16         diagnostics[2];         //4  diagnostic flags and data, usage TBD

    uint8          bulk_aggregates;        //  The aggregate functions to run on the entire span
    uint8          interval_aggregates;    //  The aggregate functions to run on each interval

    uint8          interval_type:4;        //  0: second, 1: minute, 2: hour, 3: day
    uint8          interval_step:4;        //
    uint8          interval_count;         //  Up to 256 'intervals' each aggregated individually
    // 102 bytes for data
} report_header;

typedef struct {
  uint16 count;
  uint16 min;
  uint16 max;
  uint16 sum;
} agg_counters;

// TODO: Save to flash, support dynamic configuration (per comm module and/or per sensor)
static char report_server_gsm_address[16] = {'+','1','4','1','5','9','9','2','8','3','7','0','\0','\0',0,0};
static uint16          current_sequence    = 0;
static AlarmRepeatTime report_interval     = kEveryMinute;
static uint16          report_flags        = kReportFlagDefault;
static uint8           bulk_aggregates     = kAggCount | kAggMin | kAggMax;
static uint8           interval_aggregates = kAggCount | kAggMean;

void update_interval_headers( report_header* header, AlarmRepeatTime interval )
{
  switch ( interval )
  {
    case kEvery10Seconds:
      header->interval_type = kReportIntervalSecond;
      header->interval_step = 1;
      header->interval_count = 10;
      break;
    case kEveryMinute:
      header->interval_type = kReportIntervalSecond;
      header->interval_step = 6;
      header->interval_count = 10;
      break;
    case kEvery10Minutes:
      header->interval_type = kReportIntervalMinute;
      header->interval_step = 1;
      header->interval_count = 10;
      break;
    case kEveryHour:
      header->interval_type = kReportIntervalMinute;
      header->interval_step = 6;
      header->interval_count = 10;
      break;
    case kEveryDay:
      header->interval_type = kReportIntervalHour;
      header->interval_step = 1;
      header->interval_count = 24;
      break;
    default:
      //Do nothing, error
      break;
  }
}

int32 create_time_delta( const report_header* header )
{
  switch ( header->interval_type )
  {
    case kReportIntervalSecond:
      return header->interval_step * header->interval_count;
    case kReportIntervalMinute:
      return 60 * header->interval_step * header->interval_count;
    case kReportIntervalHour:
      return 3600L * header->interval_step * header->interval_count;
  }
  return 0;
}

static uint8 agg_size( uint8 agg_set )
{
  uint8 mask = 0x1;
  uint8 size = 0;

  while ( mask != 0 )
  {
    if ( agg_set & mask )
      size += 1;
    mask = mask << 1;
  }
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
static void finish_agg( agg_counters* agg, uint8 agg_set, uint16** target )
{
  if ( agg_set & kAggCount )
  {
    **target = agg->count;
    ++(*target);
  }
  if ( agg_set & kAggSum )
  {
    **target = agg->sum;
    ++(*target);
  }
  if ( agg_set & kAggMean )
  {
    **target = (agg->count==0)? 0 : agg->sum / agg->count;
    ++(*target);
  }
  if ( agg_set & kAggMin )
  {
    **target = agg->min;
    ++(*target); 
  }
  if ( agg_set & kAggMax )
  {
    **target = agg->max;
    ++(*target);
  }
}

bool construct_report()
{
  report_header* header = (report_header*) report_buffer;

  header->report_version = 2;
  header->sensor_id = 0;
  header->sequence = current_sequence++;
  header->flags = report_flags;
  header->battery_voltage = last_battery_voltage;
  header->diagnostics[0] = sensor_event_log_count();
  header->diagnostics[1] = 0;
  header->bulk_aggregates = bulk_aggregates;
  header->interval_aggregates = interval_aggregates;

  update_interval_headers( header, report_interval );

  int32 time_delta = create_time_delta( header );
  int32 time_step = time_delta / header->interval_count;

  agg_counters bulk_agg;
  init_agg( &bulk_agg );

  agg_counters int_agg;
  init_agg( &int_agg );

  rtcc_timestamp now;
  rtcc_get_timestamp( &now );

  uint8 i, c, bucket = 0;
  uint16* bulk_ptr = (uint16*) ( report_buffer + sizeof(report_header) );
  uint16* bucket_ptr = bulk_ptr + agg_size( bulk_aggregates );
  
  do
  {
    c = read_sensor_events( event_buffer, EVENT_BUFFER_SIZE );
    for ( i = 0; i < c; ++i )
    {
      if ( header->sensor_id == 0 )
        header->sensor_id = event_buffer[i].module;
      else if ( header->sensor_id != event_buffer[i].module )
        continue; // TODO: Support multiple sensor streams
      if ( bulk_aggregates != kAggNone)
      {
        update_agg( &bulk_agg, &event_buffer[i] );
      }

      if ( interval_aggregates != kAggNone )
      {
        if ( (BYTE*)(bucket_ptr + agg_size(interval_aggregates)) - report_buffer > RAW_REPORT_MAX_LENGTH )
          continue; //TODO: Extend to a second SMS

        int32 time_seconds = rtcc_timestamp_difference( &event_buffer[i].timestamp, &now );
        if ( time_seconds > time_delta )
        {
          // This event is too old, drop it
          // TODO: extend the report start backwards to pick up the dropped events?
          continue;
        }

        if ( time_seconds < 0 )
        {
          // These will go in the next report
          requeue_sensor_events( c - i );
          c = 0;
          break;
        }

        uint8 new_bucket = header->interval_count - 1 - ( time_seconds / time_step );
        while ( bucket < new_bucket )
        {
          finish_agg( &int_agg, interval_aggregates, &bucket_ptr );
          init_agg( &int_agg );
          ++bucket;
        }
        update_agg( &int_agg, &event_buffer[i] );
      }
    }
  }
  while ( c > 0 );

  if ( interval_aggregates != kAggNone )
  {
    while ( bucket < header->interval_count )
    {
      if ( (BYTE*)(bucket_ptr + agg_size(interval_aggregates)) - report_buffer > RAW_REPORT_MAX_LENGTH )
        break; //TODO: Extend to a second SMS

      finish_agg( &int_agg, interval_aggregates, &bucket_ptr );
      init_agg( &int_agg );
      ++bucket;
    }
  }

  if ( bulk_aggregates != kAggNone)
  {
    finish_agg( &bulk_agg, bulk_aggregates, &bulk_ptr );
  }

  i = base64_encode( (const BYTE*)report_buffer, (BYTE*)bucket_ptr - report_buffer, base64_report_buffer, BASE64_REPORT_MAX_LENGTH );
  base64_report_buffer[i] = '\0';
  return true;
}



static uint8 report_stream_offset;
void receive_gsm_stream_response(unsigned char a);
void stream_to_gsm() {
  uint8 gsm_address = 11;
  MIBUnified cmd;

  if ( report_stream_offset >= strlen(base64_report_buffer) )
  {
    cmd.address = gsm_address;
    cmd.bus_command.feature = 11;
    cmd.bus_command.command = 2;
    cmd.bus_command.param_spec = plist_empty();
    bus_master_rpc_async(NULL, &cmd); //TODO: Handle failure to send
    return;
  }

  uint8 byte_count = strlen(base64_report_buffer)-report_stream_offset;
  if ( byte_count > kBusMaxMessageSize )
    byte_count = kBusMaxMessageSize;
  memcpy( &cmd.mib_buffer, base64_report_buffer+report_stream_offset, byte_count );
  report_stream_offset += byte_count;

  cmd.address = gsm_address;
  cmd.bus_command.feature = 11;
  cmd.bus_command.command = 1;
  cmd.bus_command.param_spec = plist_with_buffer(0,byte_count);

  bus_master_rpc_async( receive_gsm_stream_response, &cmd);
}
void receive_gsm_stream_response(unsigned char a) 
{
  if ( a != kNoMIBError ) {
    return;
  }
  
  taskloop_add( stream_to_gsm, NULL );
}

static ScheduledTask report_task;
void post_report( void* arg ) 
{
  if (!construct_report( report_interval ))
    return; //TODO: Log failure

  report_stream_offset = 0;

  //TODO: Get address of comm module, turn on GSM modem
  MIBUnified cmd;

  cmd.address = 11;
  cmd.bus_command.feature = 11;
  cmd.bus_command.command = 0;
  cmd.bus_command.param_spec = plist_with_buffer(0,strlen(report_server_gsm_address));
  memcpy( cmd.mib_buffer, report_server_gsm_address, strlen(report_server_gsm_address) );
  bus_master_rpc_async( receive_gsm_stream_response, &cmd);
}

void start_report_scheduling() {
    scheduler_schedule_task( post_report, report_interval, kScheduleForever, &report_task, NULL );
}
void stop_report_scheduling() {
    scheduler_remove_task( &report_task );
}
void set_report_scheduling_interval( AlarmRepeatTime interval ) {
  if ( interval >= kNumAlarmTimes )
    return;
  report_interval = interval;
  if ( BIT_TEST(report_task.flags, kBeingScheduledBit) )
    scheduler_schedule_task( post_report, report_interval, kScheduleForever, &report_task, NULL );
}
void set_report_server_gsm_address( const char* address, uint8 len )
{
  if ( len >= sizeof(report_server_gsm_address) )
    return;
  memcpy( report_server_gsm_address, address, len );
  report_server_gsm_address[len] = '\0';
}