#include "report_mib_feature.h"
#include "mib_feature_definition.h"
#include "report_manager.h"
#include "protocol.h"
#include "bus_slave.h"

static void start_scheduled_reporting(void)
{
	start_report_scheduling();
}

static void stop_scheduled_reporting(void)
{
	stop_report_scheduling();
}

static void send_report(void)
{
	taskloop_add( post_report, NULL );
}

static void set_reporting_interval(void)
{
	set_report_scheduling_interval( plist_get_int16(0) );
}

static void set_reporting_gsm_address(void)
{
	set_report_server_gsm_address( (const char*)plist_get_buffer(0), plist_get_buffer_length() );
}

DEFINE_MIB_FEATURE_COMMANDS(reporting) {
	{ 0x00, send_report, plist_spec_empty() },
	{ 0x01, start_scheduled_reporting, plist_spec_empty() },
	{ 0x02, stop_scheduled_reporting, plist_spec_empty() },
	{ 0x03, set_reporting_interval, plist_spec(1, false) },
	{ 0x04, set_reporting_gsm_address, plist_spec(0, true) }
};
DEFINE_MIB_FEATURE(reporting);
