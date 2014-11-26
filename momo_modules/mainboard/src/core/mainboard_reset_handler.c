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
#include "rn4020.h"

static bool mclr_triggered;
void handle_all_resets_before(unsigned int type)
{
    //Add code here that should be called before all other reset code
    disable_unneeded_peripherals();
    perf_enable_profiling();
    mem_init();
    mem_ensure_powered(1);
    configure_interrupts();

    taskloop_init();
    taskloop_set_flag(kTaskLoopSleepBit, 1);
    scheduler_init();
    
    con_init();

    init_mainboard_mib();
    flash_memory_init();

    //The RTCC must be enabled for scheduling tasks, so ensure that happens.
    //All modules that need to schedule tasks MUST BE called after
    //rtcc is on and enabled. 
    if (!rtcc_enabled() || _RTCLK != 0b01)
    {
        CRITICAL_LOGL( "RTCC Off, enabling.");
        configure_rtcc();
        enable_rtcc();
    }

    mclr_triggered = false;
    CRITICAL_LOGL( "Device reset.");
}

void handle_all_resets_after(unsigned int type)
{
    /*
     * Add code that should be called after all other reset code here
     */

    battery_init();
    bt_init();
    //report_manager_start();

    CRITICAL_LOGL( "Device initialized." );
}

void handle_poweron_reset(unsigned int type)
{
    CRITICAL_LOGL( "Device powered on." );
}

void handle_mclr_reset(unsigned int type)
{
    CRITICAL_LOGL( "MCLR triggered." );
    mclr_triggered = true;
}

void register_reset_handlers()
{
    register_reset_handler( kAllResetsBefore, handle_all_resets_before );
    register_reset_handler( kAllResetsAfter, handle_all_resets_after );
    register_reset_handler( kPowerOnReset, handle_poweron_reset );
    register_reset_handler( kMCLRReset, handle_mclr_reset );
}