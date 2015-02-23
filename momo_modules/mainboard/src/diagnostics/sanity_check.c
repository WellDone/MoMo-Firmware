#include "sanity_check.h"
#include "scheduler.h"
#include "module_manager.h"
#include "system_log.h"
#include "log_definitions.h"
#include "bus_master.h"
#include "rtcc.h"

ScheduledTask sanity_check_task;

void sanity_check_schedule()
{
	scheduler_schedule_task( sanity_check_run, kEveryHour, kScheduleForever, &sanity_check_task, NULL );
}

void sanity_check_run( void* arg )
{
	uint8 count = module_count();
	
	rtcc_timestamp now = rtcc_get_timestamp();
	TimeIntervalDirection dir;
	uint32 diff = 0;
	if ( bus_master_rpc_start_timestamp() != 0 )
		diff = rtcc_timestamp_difference( bus_master_rpc_start_timestamp(), now, &dir );

	if ( count != SANITY_CHECK_MODULE_COUNT 
	  || diff > 60 * SANITY_CHECK_HUNG_BUS_TIMEOUT_M )
	{
		LOG_CRITICAL(kSanityCheckResetNotice);
		LOG_INT(SANITY_CHECK_MODULE_COUNT);
		LOG_INT(count);
		LOG_INT(diff);
		LOG_FLUSH();
		asm volatile("reset");
	}
}