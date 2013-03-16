/* 
 * File:   sms_commands.c
 * Author: Austin
 *
 * Created on November 18, 2012, 11:50 PM
 */

#include "uart.h"
#include "rtcc.h"
#include "common.h"

int gsm_send_at_cmd( const char* cmd )
{
    sends( U1, cmd );
    sends( U1, "\r\n" );
}

void gsm_send_sms( const char* destination, const char* message )
{
    sends(U1, "AT+CMGS=\"");
    sends(U1, destination );
    sends(U1, "\"\r");
    wait( 300 ); // TODO: Wait for the > char on U1
    sends( U1, message );
    put( U1, 0x1A ); // send ctrl-z
}

void gsm_on()
{
    _GSM_MODULE_POWER_ON();
    wait( 300 );
    _GSM_MODULE_ON = 0;
}

void gsm_off()
{
    _GSM_MODULE_POWER_OFF();
}