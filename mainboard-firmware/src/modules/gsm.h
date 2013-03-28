//gsn.h

#ifndef __gsm_h__
#define __gsm_h__

void gsm_send_at_cmd( const char* cmd );
void gsm_send_sms( const char* destination, const char* message );

void gsm_on();
void gsm_off();

#endif //__gsm_h__
