#include "registration.h"
#include "gsm.h"
#include "momo_config.h"

void momo_register() {
  // if ( !current_momo_state.registered ) {//For now, always re-register
  gsm_on();
  gsm_send_sms( MOMO_REPORT_SERVER, "I want to register." );
  gsm_off();
  current_momo_state.registered = true;
  save_momo_state();
  //}
}