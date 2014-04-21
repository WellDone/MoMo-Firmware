#ifndef __report_manager_h__
#define	__report_manager_h__

#define MOMO_REPORT_VERSION 2

enum {
	kReportFlagDefault     = 0b0000,
	kReportFlagVerbose     = 0b0001, // By default we run a series of interval aggregates, this flag says just dump all the events in raw
	kReportFlagBigValues   = 0b0010 // By default all values and aggregates are assumed to be smaller than 2^16
};

enum {
	kReportIntervalSecond      = 0,
	kReportIntervalMinute      = 1,
	kReportIntervalHour        = 2,
	kReportIntervalDay         = 3
};

enum {
	kAggNone     = 0,
	kAggCount    = 0b00000001,
	kAggSum      = 0b00000010,
	kAggMean     = 0b00000100,
	//kAggMedian   = 0b00001000,
	//kAggVariance = 0b00010000,
	kAggMin      = 0b00100000,
	kAggMax      = 0b01000000
};

#include "rtcc.h"

void create_report();
void post_report();
void start_report_scheduling();
void stop_report_scheduling();
void set_report_scheduling_interval( AlarmRepeatTime interval );
void set_report_server_gsm_address( const char* address, uint8 len );

#endif	/* __report_manager_h__ */
