#include "report_manager.h"
#include "gsm.h"
#include "momo_config.h"

void post_report( sensor_event_log* log) {
  gsm_send_sms( SERVER_ADDRESS, "TODO: read this from the log." );
}

void start_report_scheduling() {

}