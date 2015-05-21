#include "report_mib_feature.h"
#include "mib_feature_definition.h"
#include "report_manager.h"
#include "protocol.h"
#include "bus_slave.h"
#include "momo_config.h"
#include "report_log.h"
#include "report_comm_stream.h"
#include "system_log.h"
#include <string.h>

#define BASE64_REPORT_MAX_LENGTH  160 //( 4 * ( ( RAW_REPORT_MAX_LENGTH + 2 ) / 3) )

extern char base64_report_buffer[BASE64_REPORT_MAX_LENGTH+1];


static uint8_t start_scheduled_reporting(uint8_t length)
{
	set_momo_state_flag( kStateFlagReportingEnabled, true );
	start_report_scheduling();

	return kNoErrorStatus;
}

static uint8_t stop_scheduled_reporting(uint8_t length)
{
	set_momo_state_flag( kStateFlagReportingEnabled, false );
	stop_report_scheduling();

	return kNoErrorStatus;
}

static uint8_t get_scheduled_reporting(uint8_t length)
{
	bus_slave_return_int16( get_momo_state_flag( kStateFlagReportingEnabled ) );

	return kNoErrorStatus;
}

static uint8_t send_report(uint8_t length)
{
	taskloop_add( post_report, NULL );

	return kNoErrorStatus;
}

static uint8_t set_reporting_interval(uint8_t length)
{
	set_report_scheduling_interval( plist_get_int16(0) );

	return kNoErrorStatus;
}
static uint8_t get_reporting_interval(uint8_t length)
{
	bus_slave_return_int16( current_momo_state.report_config.report_interval );

	return kNoErrorStatus;
}

static uint8_t set_reporting_route(uint8_t length)
{
	update_report_route( (plist_get_int16(0) >> 8) & 0xFF
	                   , plist_get_int16(0) & 0xFF
	                   , (const char*)plist_get_buffer(1)
	                   , plist_get_buffer_length() );

	return kNoErrorStatus;
}
static uint8_t get_reporting_route(uint8_t length)
{
	const char* route = get_report_route( ( plist_get_int16(0) >> 8 ) & 0xFF );
	if ( !route )
	{
		return kUnknownError;
	}

	uint8 start = plist_get_int16(0) & 0xFF;
	uint8 len = strlen( route ) - start;
	if ( len < 0 )
	{
		return kCallbackError;
	}

	if ( len > kMIBBufferSize )
		len = kMIBBufferSize;

	bus_slave_return_buffer( route + start, len );

	return kNoErrorStatus;
}

static uint8_t set_reporting_apn(uint8_t length)
{
	set_gprs_apn( (const char*)plist_get_buffer(0), plist_get_buffer_length() );
	return kNoErrorStatus;
}
static uint8_t get_reporting_apn(uint8_t length)
{
	bus_slave_return_buffer( current_momo_state.report_config.gprs_apn, strlen(current_momo_state.report_config.gprs_apn) );
	return kNoErrorStatus;
}

static uint8_t set_reporting_flags(uint8_t length)
{
	current_momo_state.report_config.report_flags = plist_get_int16(0);	
	return kNoErrorStatus;
}
static uint8_t get_reporting_flags(uint8_t length)
{
	bus_slave_return_int16( current_momo_state.report_config.report_flags );
	return kNoErrorStatus;
}

static uint8_t set_reporting_aggregates(uint8_t length)
{
	current_momo_state.report_config.bulk_aggregates = plist_get_int16(0);	
	current_momo_state.report_config.interval_aggregates = plist_get_int16(1);	
	return kNoErrorStatus;
}

static uint8_t get_reporting_aggregates(uint8_t length)
{
	plist_set_int16( 0, current_momo_state.report_config.bulk_aggregates );
	plist_set_int16( 1, current_momo_state.report_config.interval_aggregates );
	bus_slave_set_returnbuffer_length(4);

	return kNoErrorStatus;
}

static uint8_t build_report(uint8_t length)
{
	construct_report();
	return kNoErrorStatus;
}

static uint8_t get_report(uint8_t length)
{
	uint16 offset = plist_get_int16(0);
	memcpy(plist_get_buffer(0), base64_report_buffer+offset, 20);
	bus_slave_set_returnbuffer_length(20);

	return kNoErrorStatus;
}

static uint8_t get_reporting_sequence(uint8_t length)
{
	bus_slave_return_int16( current_momo_state.report_config.transmit_sequence );

	return kNoErrorStatus;
}

static BYTE report_buffer[RAW_REPORT_MAX_LENGTH];
static uint8_t read_report_log_mib(uint8_t call_length)
{
	uint16 index = plist_get_int16(0);
	uint16 offset = plist_get_int16(1);
	if ( offset >= RAW_REPORT_MAX_LENGTH )
	{
		return kCallbackError;
	}

	uint8 length = RAW_REPORT_MAX_LENGTH - offset;

	if ( length > kMIBBufferSize )
		length = kMIBBufferSize;

	if ( report_log_read( index, (void*)&report_buffer, 1 ) == 0 )
	{
		// No more entries
		return kCallbackError;
	}

	bus_slave_return_buffer( report_buffer+offset, length );

	return kNoErrorStatus;
}

static uint8_t count_report_log_mib(uint8_t length)
{
	bus_slave_return_int16( report_log_count() );
	return kNoErrorStatus;
}

static uint8_t clear_report_log_mib(uint8_t length)
{
	report_log_clear();
	return kNoErrorStatus;
}

static uint8_t handle_report_stream_success(uint8_t length)
{
	notify_report_success();
	return kNoErrorStatus;
}

static uint8_t handle_report_stream_failure(uint8_t length)
{
	notify_report_failure();
	return kNoErrorStatus;
}

DEFINE_MIB_FEATURE_COMMANDS(reporting) {
	{ 0x00, send_report},
	{ 0x01, start_scheduled_reporting},
	{ 0x02, stop_scheduled_reporting},
	{ 0x03, set_reporting_interval},
	{ 0x04, get_reporting_interval},
	{ 0x05, set_reporting_route},
	{ 0x06, get_reporting_route},
	{ 0x07, set_reporting_flags},
	{ 0x08, get_reporting_flags},
	{ 0x09, set_reporting_aggregates},
	{ 0x0A, get_reporting_aggregates},
	{ 0x0B, get_reporting_sequence},
	{ 0x0C, build_report},
	{ 0x0D, get_report},
	{ 0x0E, get_scheduled_reporting},
	{ 0x0F, read_report_log_mib},
	{ 0x10, count_report_log_mib},
	{ 0x11, clear_report_log_mib},
	{ 0x12, init_report_config},
	{ 0x13, set_reporting_apn},
	{ 0x14, get_reporting_apn},
	{ 0xF0, handle_report_stream_success},
	{ 0xF1, handle_report_stream_failure}
};
DEFINE_MIB_FEATURE(reporting);
