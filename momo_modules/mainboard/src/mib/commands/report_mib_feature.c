#include "report_mib_feature.h"
#include "mib_feature_definition.h"
#include "report_manager.h"
#include "protocol.h"
#include "bus_slave.h"
#include "momo_config.h"
#include "report_log.h"
#include "report_comm_stream.h"
#include <string.h>

#define BASE64_REPORT_MAX_LENGTH  160 //( 4 * ( ( RAW_REPORT_MAX_LENGTH + 2 ) / 3) )

extern char base64_report_buffer[BASE64_REPORT_MAX_LENGTH+1];


static void start_scheduled_reporting(void)
{
	set_momo_state_flag( kStateFlagReportingEnabled, true );
	start_report_scheduling();
}

static void stop_scheduled_reporting(void)
{
	set_momo_state_flag( kStateFlagReportingEnabled, false );
	stop_report_scheduling();
}

static void get_scheduled_reporting(void)
{
	bus_slave_return_int16( get_momo_state_flag( kStateFlagReportingEnabled ) );
}

static void send_report(void)
{
	taskloop_add( post_report, NULL );
}

static void set_reporting_interval(void)
{
	set_report_scheduling_interval( plist_get_int16(0) );
}
static void get_reporting_interval(void)
{
	bus_slave_return_int16( current_momo_state.report_config.report_interval );
}

static void set_reporting_gsm_address(void)
{
	set_report_server_address( (const char*)plist_get_buffer(0), plist_get_buffer_length() );
}
static void get_reporting_gsm_address(void)
{
	bus_slave_return_buffer( current_momo_state.report_config.report_server_address, strlen( current_momo_state.report_config.report_server_address ) );
}

static void set_reporting_flags(void)
{
	current_momo_state.report_config.report_flags = plist_get_int16(0);	
}
static void get_reporting_flags(void)
{
	bus_slave_return_int16( current_momo_state.report_config.report_flags );
}

static void set_reporting_aggregates(void)
{
	current_momo_state.report_config.bulk_aggregates = plist_get_int16(0);	
	current_momo_state.report_config.interval_aggregates = plist_get_int16(1);	
}
static void get_reporting_aggregates(void)
{
	plist_set_int16( 0, current_momo_state.report_config.bulk_aggregates );
	plist_set_int16( 1, current_momo_state.report_config.interval_aggregates );
	bus_slave_setreturn( pack_return_status( kNoMIBError, 4 ) );
}

static void build_report()
{
	construct_report();
}

static void get_report()
{
	uint16 offset = plist_get_int16(0);
	memcpy(plist_get_buffer(0), base64_report_buffer+offset, 20);
	bus_slave_setreturn( pack_return_status(kNoMIBError, 20 ));
}

static void get_reporting_sequence(void)
{
	bus_slave_return_int16( current_momo_state.report_config.current_sequence );
}

static BYTE report_buffer[RAW_REPORT_MAX_LENGTH];
static void read_report_log_mib(void)
{
	uint16 index = plist_get_int16(0);
	uint16 offset = plist_get_int16(1);
	if ( offset >= RAW_REPORT_MAX_LENGTH )
	{
		bus_slave_seterror( kCallbackError );
		return;
	}
	uint8 length = RAW_REPORT_MAX_LENGTH - offset;
	if ( length > kBusMaxMessageSize )
		length = kBusMaxMessageSize;

	if ( read_report_log( index, (void*)&report_buffer, 1 ) == 0 )
	{
		// No more entries
		bus_slave_seterror( kCallbackError );
		return;
	}

	bus_slave_return_buffer( report_buffer+offset, length );
}

static void update_report_stream_status(void)
{
	if ( plist_get_int16(0) == 0 )
		notify_report_success();
	else
		notify_report_failure();
}

DEFINE_MIB_FEATURE_COMMANDS(reporting) {
	{ 0x00, send_report, plist_spec_empty() },
	{ 0x01, start_scheduled_reporting, plist_spec_empty() },
	{ 0x02, stop_scheduled_reporting, plist_spec_empty() },
	{ 0x03, set_reporting_interval, plist_spec(1, false) },
	{ 0x04, get_reporting_interval, plist_spec(0, false) },
	{ 0x05, set_reporting_gsm_address, plist_spec(0, true) },
	{ 0x06, get_reporting_gsm_address, plist_spec(0, false) },
	{ 0x07, set_reporting_flags, plist_spec(1, false) },
	{ 0x08, get_reporting_flags, plist_spec(0, false) },
	{ 0x09, set_reporting_aggregates, plist_spec(2, false) },
	{ 0x0A, get_reporting_aggregates, plist_spec(0, false) },
	{ 0x0B, get_reporting_sequence, plist_spec(0, false) },
	{ 0x0C, build_report, plist_spec_empty() },
	{ 0x0D, get_report, plist_spec(1, false) },
	{ 0x0E, get_scheduled_reporting, plist_spec_empty() },
	{ 0x0F, read_report_log_mib, plist_spec(2, false) },
	{ 0x10, update_report_stream_status, plist_spec(1, false) }
};
DEFINE_MIB_FEATURE(reporting);
