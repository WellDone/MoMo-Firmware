#include "report.h"
#include "../gsm.h"

#define SERVER_ADDRESS "+17078159250"

void postReport( sensor_event_log* log) {
    gsm_send_sms( SERVER_ADDRESS, "TODO: read this from the log." );
}