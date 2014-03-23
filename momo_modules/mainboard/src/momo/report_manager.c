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
typedef struct { //103
    unsigned char report_version;
    unsigned char current_hour;
    unsigned int  battery_voltage; //2
    unsigned char hour_count;
    unsigned char event_count;
    unsigned char sensor_type;
    unsigned char __unused__;
    unsigned long hourly_buckets[24]; //96
} sms_report;

// base64 length = 4 * ( ( sizeof(sms_report) + 2 ) / 3)
#define BASE64_REPORT_LENGTH 140
static char base64_report_buffer[BASE64_REPORT_LENGTH+1];
static char report_server_gsm_address[16] = {'+','1','6','5','0','6','6','9','5','2','2','5','\0','\0',0,0};
extern unsigned int last_battery_voltage;

static sensor_event event_buffer[EVENT_BUFFER_SIZE];
bool construct_report()
{
  sms_report report;
  unsigned int count, i;
  rtcc_datetime time1, time2;
  rtcc_get_time( &time2 );

  report.report_version = MOMO_REPORT_VERSION;
  report.current_hour = time2.hours;
  report.battery_voltage = last_battery_voltage;

  for ( i=0; i<24; ++i ) {
    report.hourly_buckets[i] = 0;
  }
  report.hour_count = 0;
  report.event_count = 0;
  report.sensor_type = 0;

  /*count = read_sensor_events( event_buffer, 1 );

  if ( count != 0 ) {
    report.sensor_type = event_buffer[0].type;

    time1.year = event_buffer[i].starttime.date.year;
    time1.month = event_buffer[i].starttime.date.month;
    time1.day = event_buffer[i].starttime.date.day;
    time1.hours = event_buffer[i].starttime.hour;
    time1.minutes = event_buffer[i].starttime.minute;
    time1.seconds = 0;

    rtcc_datetime_difference( &time1, &time2 );
    report.hour_count = 24 * time2.day + time2.hours+1;
  }
  while ( count != 0 ) {
    if ( event_buffer[0].type != report.sensor_type )
      continue;
    report.event_count += 1;
    report.hourly_buckets[event_buffer[0].starttime.hour] += event_buffer[0].value;

    count = read_sensor_events( event_buffer, 1 );
  }*/

  report.event_count = 20;

  count = base64_encode( (BYTE*)&report, 104, base64_report_buffer, BASE64_REPORT_LENGTH );
  base64_report_buffer[count] = '\0';
  return true;
}

static uint8 report_stream_offset;
void receive_gsm_stream_response(unsigned char a);
void stream_to_gsm() {
  uint8 gsm_address = 11;
  MIBUnified cmd;

  if ( report_stream_offset >= BASE64_REPORT_LENGTH )
  {
    cmd.address = gsm_address;
    cmd.bus_command.feature = 11;
    cmd.bus_command.command = 2;
    cmd.bus_command.param_spec = plist_empty();
    bus_master_rpc_async(NULL, &cmd); //TODO: Handle failure to send
    return;
  }

  uint8 byte_count = BASE64_REPORT_LENGTH-report_stream_offset;
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
void receive_gsm_stream_response(unsigned char a) {
  if ( a != kNoMIBError ) {
    return;
  }
  
  taskloop_add( stream_to_gsm );
}
void post_report() 
{
  //TODO: Gather sensor data from sensor modules
  if (!construct_report())
    return;

  report_stream_offset = 0;

  //TODO: Get address of GSM module.
  uint8 gsm_address = 11;
  MIBUnified cmd;

  cmd.address = gsm_address;
  cmd.bus_command.feature = 11;
  cmd.bus_command.command = 0;
  cmd.bus_command.param_spec = plist_with_buffer(0,strlen(report_server_gsm_address));
  memcpy( cmd.mib_buffer, report_server_gsm_address, strlen(report_server_gsm_address) );
  bus_master_rpc_async( receive_gsm_stream_response, &cmd);
}

static ScheduledTask report_task;
static AlarmRepeatTime report_interval = kEvery10Seconds;
void start_report_scheduling() {
    scheduler_schedule_task( post_report, report_interval, kScheduleForever, &report_task);
}
void stop_report_scheduling() {
    scheduler_remove_task( &report_task );
}
void set_report_scheduling_interval( AlarmRepeatTime interval ) {
  if ( interval >= kNumAlarmTimes )
    return;
  report_interval = interval;
  if ( BIT_TEST(report_task.flags, kBeingScheduledBit) )
    scheduler_schedule_task( post_report, report_interval, kScheduleForever, &report_task);
}
void set_report_server_gsm_address( const char* address, uint8 len )
{
  if ( len >= sizeof(report_server_gsm_address) )
    return;
  memcpy( report_server_gsm_address, address, len );
  report_server_gsm_address[len] = '\0';
}