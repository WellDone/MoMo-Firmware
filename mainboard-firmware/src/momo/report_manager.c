#include "report_manager.h"
#include "gsm.h"
#include "rtcc.h"
#include "momo_config.h"
#include "sensor_event_log.h"

#define EVENT_BUFFER_SIZE 24
typedef struct { //103
  unsigned char momo_version;
  unsigned int  battery_voltage; //2
  unsigned char sensor_type;
  rtcc_date     date; //3
  unsigned long  hourly_buckets[24]; //96
} sms_report;

extern unsigned int last_battery_voltage;

static sms_report report;
static sensor_event event_buffer[EVENT_BUFFER_SIZE];
void construct_report()
{
  unsigned int count, i;
  report.momo_version = MOMO_PRODUCTION_VERSION;
  report.battery_voltage = last_battery_voltage;

  for ( i=0; i<24; ++i ) {
    report.hourly_buckets[i] = 0;
  }

  count = read_sensor_events( event_buffer, 1 );
  if ( count == 0 )
    return;
  report.sensor_type = event_buffer[0].type;
  report.date = event_buffer[0].starttime.date;
  report.hourly_buckets[event_buffer[0].starttime.hour] += event_buffer[0].value;

  while ( !sensor_event_log_empty() ) {
    count = read_sensor_events( event_buffer, EVENT_BUFFER_SIZE );
    for ( i = 0; i<count; ++i )
    {
      if ( event_buffer[i].type != report.sensor_type )
        continue;
      report.hourly_buckets[event_buffer[0].starttime.hour] += event_buffer[0].value;
    }
  }
}

void post_report() {
  construct_report();
  gsm_send_sms( MOMO_REPORT_SERVER, (const char*)&report );
}

void start_report_scheduling() {
  set_recurring_task( MOMO_REPORT_INTERVAL, post_report );
}