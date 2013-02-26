/* 
 * File:   sms_commands.c
 * Author: Austin
 *
 * Created on November 18, 2012, 11:50 PM
 */

#include "serial_commands.h"

static int gsm_at_cmd( const char* cmd )
{
    sends( U1, cmd );
    sends( U1, "\r" );
    wait( 300 );
}

static void gsm_power_on()
{
    while ( _LATA2 == 1 )
        ; // READY goes high first
    while ( _LATA2 == 0 )
        ; // Then low again while module boots
    // Now we're ready

    gsm_at_cmd( "AT" );
    gsm_at_cmd( "AT+CMGF=1" );
}

static void send_sms( const char* da, const char* message )
{
    sends( U1, "AT+CMGS=\"+17078159250\"\r\n");
    wait( 300 );
    sends( U1, message );
    put( U1, 0x1A ); // send ctrl-z
    wait( 300 );


    sendf(U2, "SMS sent to %s: %s", da, message );
    sends(U2, "\r\n");
}

static void gsm_on()
{
    _LATA3 = 1;

    // TODO: Supply power to the module first, when that's supported
    if ( _LATA2 == 0 )
    {
      _LATA1 = 0;
      while ( _LATA2 == 1 )
        ; // READY goes high first
    while ( _LATA2 == 0 )
        ; // Then low again while module boots
      _LATA1 = 1;
      wait( 800 ); //TODO: Sleep until the "READY" light is on
      if ( _LATA2 == 0 )
      {
        sends(U1, "The GSM module failed to power up.");
        return;
      }
      sends(U1, "The GSM module is now ON!");
    }
    else
    {
      sends(U1, "The GSM module is already on.");
      return;
    }
}

static void gsm_off()
{
    if ( _LATA2 == 1 )
    {
        _LATA1 = 0;
        wait( 200 );
        _LATA1 = 1;
        wait( 500 ); //TODO: Sleep until the "READY" light is off
        if ( _LATA2 == 1 )
        {
            sends(U1, "The GSM module failed to power down.");
            return;
        }
        sends(U1, "GSM module turned off.");
    }
    else
    {
        sends(U1, "The GSM module is already off.");
    }
}

void handle_gsm(command_params *params)
{
  if (params->num_params < 1)
  {
    sends(U1, "You must pass a function (i.e. 'send') to the gsm command.\n");
    return;
  }

  char *cmd = get_param_string(params,0);

  if (strcmp("on", cmd) == 0)
  {
      gsm_on();
  }
  else if (strcmp("off", cmd) == 0)
  {
      gsm_off();
  }
  else if (strcmp("send", cmd) == 0)
  {
    if (params->num_params < 3)
    {
      sends(U1, "You must pass a number and message to the 'gsm send' command.\n");
      return;
    }
    send_sms( get_param_string(params,1), get_param_string(params,2) );
  }
  else
  {
    sends(U1, "Invalid argument to gsm command.\n");
  }
}

/*
 * 
 */
