#include "registration.h"
#include "gsm.h"
#include "momo_config.h"
#include "report_manager.h"
#include "base64.h"

typedef struct {
  unsigned char registration_marker;
  unsigned short error_count; // TODO: Actually keep track of errors and send them up when registering.
  //unsigned int battery_voltage;
} RegistrationMessage;
static RegistrationMessage registration_message;
static char base64_message_buffer[8+1];

extern unsigned int last_battery_voltage;

bool send_gsm_registration()
{
  unsigned int count;
  registration_message.registration_marker = 0x0;
  registration_message.error_count = 0x0;
  //registration_message.battery_voltage = last_battery_voltage;

  count = base64_encode( (BYTE*)&registration_message, 2, base64_message_buffer, 8 );
  base64_message_buffer[count] = '\0';

  if ( gsm_send_sms( MOMO_REPORT_SERVER, base64_message_buffer ) )
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