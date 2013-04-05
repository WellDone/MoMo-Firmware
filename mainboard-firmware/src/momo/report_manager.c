#include "report_manager.h"
#include "gsm.h"
#include "scheduler.h"
#include "momo_config.h"
#include "sensor_event_log.h"
#include "utilities.h"
#include "uart.h"
#include "base64.h"

#define EVENT_BUFFER_SIZE 10
typedef struct { //103
    unsigned char momo_version;
    unsigned char sensor_type;
    unsigned int  battery_voltage; //2
    rtcc_date     date; //3
    unsigned char boundary___;
    unsigned long  hourly_buckets[24]; //96
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
  report.momo_version = MOMO_REPORT_VERSION;
  report.battery_voltage = last_battery_voltage;

  for ( i=0; i<24; ++i ) {
    report.hourly_buckets[i] = 0;
  }

  count = read_sensor_events( event_buffer, 1 );
  if ( count == 0 )
  {
    return false;
  }
  report.sensor_type = event_buffer[0].type;
  report.date = event_buffer[0].starttime.date;
  report.hourly_buckets[event_buffer[0].starttime.hour] += event_buffer[0].value;

  while ( !sensor_event_log_empty() ) {
    count = read_sensor_events( event_buffer, EVENT_BUFFER_SIZE );
    for ( i = 0; i<count; ++i )
    {
      if ( event_buffer[i].type != report.sensor_type )
        continue;
      report.hourly_buckets[event_buffer[i].starttime.hour] += event_buffer[i].value;
    }
  }

  count = base64_encode( (BYTE*)&report, 104, base64_report_buffer, BASE64_REPORT_LENGTH );
  base64_report_buffer[count] = '\0';
  return (count > 0);
}

void post_report() {
  if (!construct_report())
    return;

  gsm_on();
  gsm_send_sms( MOMO_REPORT_SERVER, base64_report_buffer ); //TODO: Retry on failure.
  gsm_off();
}

static ScheduledTask report_task = {0, 0, 0, 0};
void start_report_scheduling() {
    scheduler_schedule_task( post_report, MOMO_REPORT_INTERVAL, kScheduleForever, &report_task);
}
void stop_report_scheduling() {
    scheduler_remove_task( &report_task );
}