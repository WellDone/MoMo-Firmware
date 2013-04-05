#include "registration.h"
#include "gsm.h"
#include "momo_config.h"
#include "report_manager.h"

bool send_gsm_registration()
{
  if ( gsm_send_sms( "+17078159250", "I want to register." ) )
  {
    current_momo_state.registered = true;
    save_momo_state();
    return true;
  }
  return false;
}

bool attempt_registration()
{
  return send_gsm_registration();
}

bool momo_register_and_start_reporting()
{
  bool success = true;
  if ( gsm_on() )
  {
    //if ( !current_momo_state.registered ) { //For now, always re-register
      if ( attempt_registration() )
      {
        success = true;
      } else {
        success = false;
      }
    //}
    gsm_off();
  }

  if (success)
    start_report_scheduling();

  return success;
}