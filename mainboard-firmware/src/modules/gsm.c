#include "gsm.h"
#include "uart.h"
#include "rtcc.h"
#include "common.h"
#include "pme.h"
#include "gsm.h"
#include <string.h>

// Input/Output and Interrupt PIN definitions
#define GSM_POWER_PIN       _LATA0
#define GSM_POWER_TRIS      _TRISA0
#define GSM_POWER_ON()      GSM_POWER_PIN = 1
#define GSM_POWER_OFF()     GSM_POWER_PIN = 0

#define GSM_MODULE_ON_PIN   _LATA2
#define GSM_MODULE_ON_TRIS  _TRISA2
#define GSM_MODULE_ON_OD    _ODA2
#define GSM_MODULE_ON()     GSM_MODULE_ON_PIN = 0
#define GSM_MODULE_OFF()    GSM_MODULE_ON_PIN = 1

void gsm_disable_serial()
{
    uart_set_disabled( U1, 1 );

    //TODO: Clock leave high-speed mode
}

void gsm_init()
{
    //Set direction to out
    GSM_POWER_TRIS = 0;
    GSM_MODULE_ON_TRIS = 0;

    //Configure pins to control WISMO power and module enable
    GSM_MODULE_ON_OD = 1;
    GSM_POWER_OFF();
    GSM_MODULE_OFF();

    gsm_disable_serial();
}

void gsm_configure_serial()
{
    uart_parameters params_uart1;

    //TODO: Clock enter high-speed mode

    peripheral_enable(kUART1Module);

    params_uart1.baud = 115200;
    params_uart1.hw_flowcontrol = 0;
    params_uart1.parity = NoParity;
    configure_uart( U1, &params_uart1 );
}

<<<<<<< HEAD
void gsm_disable_serial()
{
    peripheral_disable(kUART1Module);

    //TODO: Clock leave high-speed mode
}

=======
>>>>>>> dev
void gsm_send_at_cmd( const char* cmd )
{
    sends( U1, cmd );
    sends( U1, "\r\n" );
}

void gsm_send_sms( const char* destination, const char* message )
{
    gsm_send_binary_sms( destination, (BYTE*)message, strlen( message ) );
}

void gsm_send_binary_sms( const char* destination, const BYTE* data, unsigned short length ) {
    unsigned short i;
    gsm_send_at_cmd( "AT+CMGF=1" );
    wait_ms( 20 );
    sends(U1, "AT+CMGS=\"");
    sends(U1, destination );
    sends(U1, "\"\r");
    wait_ms( 20 ); // TODO: Wait for the > char on U1
    for ( i=0; i<length; ++i )
    {
        put( U1, data[i] );
    }
    put( U1, 0x1A ); // ASCII ctrl-z = 0x1A
}

bool gsm_check_SIM()
{
    gsm_send_at_cmd( "AT+CPIN?" );
    //TODO: implement response parsing
    return true;
}

void gsm_on()
{
    gsm_configure_serial();

    GSM_POWER_ON();
    wait_ms( 300 );
    GSM_MODULE_ON();
}

GSMStatus gsm_status()
{
    GSMStatus status = gsm_status_off;
    if ( GSM_POWER_PIN == 1 && GSM_MODULE_ON_PIN == 0 )
    {
        if ( gsm_check_SIM() )
            status = gsm_status_ready;
        else
            status = gsm_status_on;
    }
    return status;
}

void gsm_off()
{
    gsm_disable_serial();
    GSM_MODULE_OFF();
    GSM_POWER_OFF();
}