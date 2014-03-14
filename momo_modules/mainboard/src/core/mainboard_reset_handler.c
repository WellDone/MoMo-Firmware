#include "mainboard_reset_handler.h"

#include "rtcc.h"
#include "uart.h"
#include "task_manager.h"
#include "scheduler.h"
#include "oscillator.h"
#include "pme.h"
#include "memory.h"
#include "bus.h"
#include "mainboard_mib_commands.h"
#include "report_manager.h"
#include "memory_manager.h"
#include "firmware_cache.h"
#include "controller_mib_feature.h"

static bool mclr_triggered;
void handle_all_resets_before(unsigned int type)
{
    //Add code here that should be called before all other reset code
    disable_unneeded_peripherals();
    configure_interrupts();
    oscillator_init();

    taskloop_init();
    scheduler_init();
    
    bus_init(kMIBControllerAddress);
    con_init();

    init_mainboard_mib();
    flash_memory_init();
    fc_init(); //depends on flash_memory_init

    mclr_triggered = false;
}

void handle_all_resets_after(unsigned int type)
{
    /*
     * Add code that should be called after all other reset code here
     */

    //The RTCC must be enabled for scheduling tasks, so ensure that
    if (!rtcc_enabled())
        enable_rtcc();

    //start_report_scheduling();
}

void handle_poweron_reset(unsigned int type)
{
    //Power-on reset resets the rtcc, so configure and enable it.
    configure_rtcc();
    enable_rtcc();
}

void handle_mclr_reset(unsigned int type)
{
    mclr_triggered = true;
}

void register_reset_handlers()
{
    register_reset_handler( kAllResetsBefore, handle_all_resets_before );
    register_reset_handler( kAllResetsAfter, handle_all_resets_after );
    register_reset_handler( kPowerOnReset, handle_poweron_reset );
    register_reset_handler( kMCLRReset, handle_mclr_reset );
}