#include "registration.h"
#include "gsm.h"
#include "momo_config.h"

void momo_register() {
  gsm_on();
  gsm_send_sms( MOMO_REPORT_SERVER, "I want to register." );
  gsm_off();
}