#ifndef __report_manager_h__
#define	__report_manager_h__

#include "platform.h"
#include "rtcc.h"

#define MOMO_REPORT_VERSION 2
#define pack_report_interval(type, step)	(((step&0xF) << 4) | (type&0xF))

// If this ever changes, make sure to update report_log.c
#define RAW_REPORT_MAX_LENGTH     118
#define BASE64_REPORT_MAX_LENGTH  160 //( 4 * ( ( RAW_REPORT_MAX_LENGTH + 2 ) / 3) )
#define NUM_BUCKETS               56

enum 
{
	kReportFlagDefault     = 0b0000,
	kReportFlagVerbose     = 0b0001, // By default we run a series of interval aggregates, this flag says just dump all the events in raw
	kReportFlagBigValues   = 0b0010 // By default all values and aggregates are assumed to be smaller than 2^16
};

enum 
{
	kReportIntervalSecond      = 0,
	kReportIntervalMinute      = 1,
	kReportIntervalHour        = 2,
	kReportIntervalDay         = 3
};

enum
{
	kReportStatusNone            = 0,
	kReportStatusConstructed     = 1,
	kReportStatusOpeningStream   = 2,
	kReportStatusStreaming       = 3,
	kReportStatusClosingStream   = 4,
	kReportStatusPending         = 5,
	kReportStatusWaitingForRetry = 6,
	kReportStatusFailed          = 0x0F,
	kReportStatusSuccess         = 0xFF
};

enum 
{
	kAggNone     = 0,
	kAggCount    = 0b00000001,
	kAggSum      = 0b00000010,
	kAggMean     = 0b00000100,
	//kAggMedian   = 0b00001000,
	//kAggVariance = 0b00010000,
	kAggMin      = 0b00100000,
	kAggMax      = 0b01000000
};

typedef struct 
{
	char            report_server_address[16];
	uint16          current_sequence;
	AlarmRepeatTime report_interval;
	uint16          report_flags;
	uint8           bulk_aggregates;
	uint8           interval_aggregates;
} ReportConfiguration;

void report_manager_start();
void init_report_config();
bool construct_report();
void post_report( void* );
void start_report_scheduling();
void stop_report_scheduling();
void set_report_scheduling_interval( AlarmRepeatTime interval );
void set_report_server_address( const char* address, uint8 len );

#endif	/* __report_manager_h__ */
