#include "report_manager.h"
#include "common.h"
#include "gsm.h"
#include "scheduler.h"
#include "momo_config.h"
#include "sensor_event_log.h"
#include "utilities.h"
#include "uart.h"
#include "base64.h"
#include <stdlib.h>

#define EVENT_BUFFER_SIZE 10
typedef struct { //103
    unsigned char momo_version;
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

extern unsigned int last_battery_voltage;

static sensor_event event_buffer[EVENT_BUFFER_SIZE];
bool construct_report()
{
  sms_report report;
  unsigned int count, i;
  rtcc_datetime time1, time2;
  rtcc_get_time( &time2 );

  report.momo_version = MOMO_REPORT_VERSION;
  report.current_hour = time2.hours;
  report.battery_voltage = last_battery_voltage;

  for ( i=0; i<24; ++i ) {
    report.hourly_buckets[i] = 0;
  }

  count = read_sensor_events( event_buffer, 1 );

  if ( count != 0 ) {
    report.event_count = 1;

    time1.year = event_buffer[i].starttime.date.year;
    time1.month = event_buffer[i].starttime.date.month;
    time1.day = event_buffer[i].starttime.date.day;
    time1.hours = event_buffer[i].starttime.hour;
    time1.minutes = event_buffer[i].starttime.minute;
    time1.seconds = 0;

    report.sensor_type = event_buffer[0].type;
    report.hourly_buckets[event_buffer[0].starttime.hour] += event_buffer[0].value;

    while ( !sensor_event_log_empty() ) {
      count = read_sensor_events( event_buffer, EVENT_BUFFER_SIZE );
      for ( i = 0; i<count; )
      {
        if ( event_buffer[i].type != report.sensor_type )
          continue;
        report.hourly_buckets[event_buffer[i].starttime.hour] += event_buffer[i].value;
        report.event_count += 1;
      }
    }

    rtcc_datetime_difference( &time1, &time2 );
    report.hour_count = 24 * time2.day + time2.hours;
  } else {
    report.hour_count = 0;
    report.event_count = 0;
  }
  count = base64_encode( (BYTE*)&report, 104, base64_report_buffer, BASE64_REPORT_LENGTH );
  base64_report_buffer[count] = '\0';
  return (count > 0);
}

void post_report() {
  if (!construct_report())
    return;

  if ( gsm_on() )
  {
    gsm_send_sms( MOMO_REPORT_SERVER, base64_report_buffer ); //TODO: Retry on failure?
    gsm_off();
  }
}

static ScheduledTask report_task = {0, 0, 0, 0};
void start_report_scheduling() {
    scheduler_schedule_task( post_report, MOMO_REPORT_INTERVAL, kScheduleForever, &report_task);
}
void stop_report_scheduling() {
    scheduler_remove_task( &report_task );
}