#include "gsm.h"
#include "uart.h"
#include "rtcc.h"
#include "pme.h"
#include "gsm.h"
#include <string.h>
#include <stdarg.h>

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

bool wait_for_response( const char* success, const char* failure );

void gsm_disable_serial()
{
    peripheral_disable( kUART1Module );

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

bool gsm_send_at_cmd( const char* cmd )
{
    sends( U1, cmd );
    sends( U1, "\r\n" );
    return wait_for_response( "OK", "ERROR" );
}

#define RESPONSE_MARKER_COUNT 4
#define RESPONSE_MARKER_MAX_LENGTH 16
static char response_buffer[RESPONSE_MARKER_COUNT][RESPONSE_MARKER_MAX_LENGTH];
static volatile bool waiting_for_response[RESPONSE_MARKER_COUNT];
static volatile unsigned int response_buffer_index[RESPONSE_MARKER_COUNT];
#define GSM_RESPONSE_TIMEOUT 8000000
void gsm_receive_char( char c ) {
    unsigned int i;
    for ( i=0; i<RESPONSE_MARKER_COUNT; ++i ) {
        if ( waiting_for_response[i] && response_buffer[i][response_buffer_index[i]] == c  ) {
            response_buffer_index[i] += 1;
            if ( response_buffer[i][response_buffer_index[i]] == '\0' ) {
                waiting_for_response[i] = false;
            }
        } else {
            response_buffer_index[i] = 0;
        }
    }
}

int register_response_marker( const char* marker ) {
    if ( marker == 0 )
        return -1;
    unsigned int i, len = strlen( marker );
    if ( len > RESPONSE_MARKER_MAX_LENGTH )
        return -1;
    for ( i=0; i<RESPONSE_MARKER_COUNT; ++i ) {
        if ( !waiting_for_response[i] ) {
            memcpy( response_buffer[i], marker, len );
            response_buffer[i][len] = '\0';
            response_buffer_index[i] = 0;
            waiting_for_response[i] = true;
            return i;
        }
    }
    return -1;
}
void remove_response_marker( int index ) {
    if ( index < 0 || index >= RESPONSE_MARKER_COUNT )
        return;
    waiting_for_response[index] = false;
}
bool wait_for_response( const char* success, const char* failure )
{
    unsigned int success_index = register_response_marker( success );
    unsigned int failure_index = register_response_marker( failure );

    if ( success_index == -1 )
        return false;

    unsigned long timeout = GSM_RESPONSE_TIMEOUT;
    while ( waiting_for_response[success_index] &&
           ( failure_index == -1 || waiting_for_response[failure_index] ) &&
           --timeout != 0 )
        ;
    bool success_found = !waiting_for_response[success_index];
    remove_response_marker( success_index );
    remove_response_marker( failure_index );
    return success_found;
}

bool gsm_send_sms( const char* destination, const char* message )
{
    int i, len;
    gsm_send_at_cmd( "AT+E=0" );
    gsm_send_at_cmd( "AT+CMGF=1" );

    sends(U1, "AT+CMGS=\"");
    sends(U1, destination );
    sends(U1, "\"\r");
    wait_for_response( ">", 0 );

    len = strlen(message);
    if ( len > 160 )
        len = 160;
    for ( i=0; i<len; ++i ) {
        put( U1, message[i] );
    }

    put( U1, 0x1A ); // ASCII ctrl-z = 0x1A

    return wait_for_response( "OK", "ERROR" );
}

bool gsm_check_SIM()
{
    int ready_index = register_response_marker( "+CPIN: READY" );
    if ( ready_index == -1 )
        return false;
    bool success = gsm_send_at_cmd( "AT+CPIN?" ) && !waiting_for_response[ready_index];
    remove_response_marker( ready_index );
    return success;
}

bool gsm_check_registered()
{
    int registered_index = register_response_marker( "+CREG: 0,1" );
    if ( registered_index == -1 )
        return false;
    bool success = gsm_send_at_cmd( "AT+CREG?" ) && !waiting_for_response[registered_index];
    remove_response_marker( registered_index );
    return success;
}

bool wait_for_ready()
{
  unsigned int retry_attempts = 30;
  if ( !gsm_check_SIM() )
    return false;

  while ( retry_attempts > 0 && !gsm_check_registered() ) {
    --retry_attempts;
    wait_ms( 200 );
  }
  return retry_attempts != 0;
}


void gsm_on_raw()
{
    gsm_configure_serial();

    GSM_POWER_ON();
    wait_ms( 300 );
    GSM_MODULE_ON();
}

bool gsm_on()
{
    gsm_on_raw();
    if ( !wait_for_response( "+PSSUP", 0 ) || !wait_for_ready() )
    {
        gsm_off();
        return false;
    }
    return true;
}

GSMStatus gsm_status()
{
    GSMStatus status = gsm_status_off;
    if ( GSM_POWER_PIN == 1 && GSM_MODULE_ON_PIN == 0 )
    {
        if ( gsm_check_SIM() ) {
            if ( gsm_check_registered() ) {
                status = gsm_status_registered;
            } else {
                status = gsm_status_ready;
            }
        } else {
            status = gsm_status_on;
        }
    }
    return status;
}

void gsm_off()
{
    gsm_disable_serial();
    GSM_MODULE_OFF();
    GSM_POWER_OFF();
}