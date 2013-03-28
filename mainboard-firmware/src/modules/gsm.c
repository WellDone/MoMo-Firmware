/*
 * File:   sms_commands.c
 * Author: Austin
 *
 * Created on November 18, 2012, 11:50 PM
 */

#include "uart.h"
#include "rtcc.h"
#include "common.h"

void configure_gsm() {
    uart_parameters params_uart1;
    _LATA0 = 0;
    _TRISA0 = 0;

    params_uart1.baud = 115200;
    params_uart1.hw_flowcontrol = 0;
    params_uart1.parity = NoParity;
    configure_uart( U1, &params_uart1 );
}

void gsm_send_at_cmd( const char* cmd )
{
    sends( U1, cmd );
    sends( U1, "\r\n" );
}

void gsm_send_sms( const char* destination, const char* message )
{
    sends(U1, "AT+CMGS=\"");
    sends(U1, destination );
    sends(U1, "\"\r");
    wait_ms( 1 ); // TODO: Wait for the > char on U1
    sends( U1, message );
    put( U1, 0x1A ); // send ctrl-z
}

void gsm_on()
{
    _GSM_MODULE_POWER_ON();
    wait_ms( 1 );
    _GSM_MODULE_ON = 0;
}

void gsm_off()
{
    _GSM_MODULE_POWER_OFF();
}