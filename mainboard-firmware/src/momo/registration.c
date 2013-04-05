#include "registration.h"
#include "gsm.h"
#include "momo_config.h"
#include "report_manager.h"

void send_gsm_registration()
{
  gsm_send_sms( "+17078159250", "I want to register." );
  current_momo_state.registered = true;
  save_momo_state();
}

bool attempt_registration()
{
  if ( gsm_check_SIM() ) {
    send_gsm_registration();
    return true;
  }
  return false;
}

bool momo_register_and_start_reporting()
{
  bool success = true;
  gsm_on();
   if ( !current_momo_state.registered ) { //For now, always re-register
    if ( attempt_registration() )
    {
      success = true;
    } else {
      success = false;
    }
  }
  gsm_off();

  if (success)
    start_report_scheduling();

  return success;
}