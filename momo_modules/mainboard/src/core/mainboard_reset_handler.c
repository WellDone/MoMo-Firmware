#include "mainboard_reset_handler.h"

#include "rtcc.h"
#include "uart.h"
#include "task_manager.h"
#include "scheduler.h"
#include "oscillator.h"
#include "pme.h"
#include "memory.h"
#include "bus.h"
#include "battery.h"
#include "mainboard_mib_commands.h"
#include "report_manager.h"
#include "memory_manager.h"
#include "controller_mib_feature.h"
#include "system_log.h"
#include "perf.h"
#include "momo_config.h"
#include "log_definitions.h"
#include "rn4020.h"

static bool mclr_triggered;
void handle_all_resets_before(unsigned int type)
{
    int rtcc_disabled = 0;
    //Add code here that should be called before all other reset code

    //mem_init must be called before any logging statements in order to ensure
    //that the system log is properly initialized.  
    mem_init();
    mem_ensure_powered(1);

    
    //The RTCC must be enabled for scheduling tasks, so ensure that happens.
    //All modules that need to schedule tasks MUST BE called after
    //rtcc is on and enabled. Use the SOSC oscillator since the internal RC
    //32.25 khz oscillator is +- 20% precision, causing lots of skew.
    if (!rtcc_enabled() || _RTCLK != kRTCCSoscSource)
    {
        configure_rtcc(kRTCCSoscSource);
        enable_rtcc();
        rtcc_disabled = 1;
    }
    
    disable_unneeded_peripherals();
    perf_enable_profiling();

    configure_interrupts();

    scheduler_init(); //must come before taskloop and any logging calls since the taskloop and log calls add a scheduled task
    taskloop_init();
    taskloop_set_flag(kTaskLoopSleepBit, 1);
    
    con_init();

    init_mainboard_mib();
    flash_memory_init();

    //Do the logging now since we need flash_memory_init() called to initialize the syslog
    if (rtcc_disabled)
        LOG_CRITICAL(kRTCCOffNotice); //log after enabling rtcc so that the timestamp makes sense

    mclr_triggered = false;
    LOG_CRITICAL(kDeviceResetNotice);
}

void handle_all_resets_after(unsigned int type)
{
    /*
     * Add code that should be called after all other reset code here
     */

    battery_init();
    //bt_init();
    report_manager_start();

    LOG_CRITICAL(kDeviceInitializedNotice);
}

void handle_poweron_reset(unsigned int type)
{
    LOG_CRITICAL(kPowerOnNotice);
}

void handle_mclr_reset(unsigned int type)
{
    LOG_CRITICAL(kMCLRTriggeredNotice);
    mclr_triggered = true;
}

void register_reset_handlers()
{
    register_reset_handler( kAllResetsBefore, handle_all_resets_before );
    register_reset_handler( kAllResetsAfter, handle_all_resets_after );
    register_reset_handler( kPowerOnReset, handle_poweron_reset );
    register_reset_handler( kMCLRReset, handle_mclr_reset );
}