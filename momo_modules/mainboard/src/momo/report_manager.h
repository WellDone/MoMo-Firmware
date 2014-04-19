#ifndef __report_manager_h__
#define	__report_manager_h__

#define MOMO_REPORT_VERSION 1

#include "rtcc.h"

void post_report();
void start_report_scheduling();
void stop_report_scheduling();
void set_report_scheduling_interval( AlarmRepeatTime interval );
void set_report_server_gsm_address( const char* address, uint8 len );

#endif	/* __report_manager_h__ */
