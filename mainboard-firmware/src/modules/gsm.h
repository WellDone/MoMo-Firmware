//gsn.h

#ifndef __gsm_h__
#define __gsm_h__

// Input/Output and Interrupt PIN definitions
#define GSM_POWER_PIN      	_LATA0
#define GSM_POWER_TRIS	   	_TRISA0
#define GSM_POWER_ON()  	GSM_POWER_PIN = 1
#define GSM_POWER_OFF() 	GSM_POWER_PIN = 0

#define GSM_MODULE_ON_PIN  	_LATA2
#define GSM_MODULE_ON_TRIS	_TRISA2
#define GSM_MODULE_ON_OD	_ODA2

void gsm_init();

int gsm_send_at_cmd( const char* cmd );
void gsm_send_sms( const char* destination, const char* message );

void gsm_on();
void gsm_off();

void gsm_configure_serial(); //Enable the UART for communicating with the GSM module

#endif //__gsm_h__
