/*
 * reset_manager.c
 * This file contains routines for determining the cause of a reset and registering
 * handlers to be called when that type of resset occurs.
 *
 */

#include <p24F16KA101.h>
#include "reset_manager.h"
#include "rtcc.h"
#include "uart.h"
#include "task_manager.h"
#include "modules/scheduler.h"

//Global reset handler table
reset_handler reset_handlers[kNumResets][MAX_RESETS_PER_TYPE] =
{
    {handle_poweron_reset},     //POR Reset
    {0},                        //Reset during sleep
    {0},                        //Wake from deep sleep
    {0},                        //Software reset
    {0},                        //MCLR External reset
    {handle_all_resets_before}, //Call on all resets (before)
    {handle_all_resets_after}   //Call after all other reset code   
};

ScheduledTask minutetask, tenminutetask, hourtask, daytask;

/*
 * register_reset_handler
 * Register a handler to be called the next time a given reset type occurs.
 * If there is not space in the handler table to add another entry, this
 * function returns -1, otherwise it returns an opaque handle that can
 * be used to remove that reset handler later.  No guarantees are made
 * about the order in which these handlers will be called when the reset
 * type occurs.
 *
 */
unsigned int register_reset_handler(ResetType type, reset_handler handler)
{
    unsigned int i;
    for (i=0;i<MAX_RESETS_PER_TYPE; ++i)
    {
        if (reset_handlers[type][i] == 0)
        {
            reset_handlers[type][i] = handler;
            return ((type << 8) | i);
        }
    }

    return INVALID_RESET_HANDLE; //There wasn't space for this reset handler.
}

/*
 * remove the reset handler associated with the given opaque handle.
 * Returns 0 on success and an error code on error.  Possible codes are:
 * -1: invalid handle
 * -2: valid handle but reset has already been removed or was never installed
 */
int remove_reset_handler(unsigned int handle)
{
    unsigned int type = (handle & 0xFF00) >> 8;
    unsigned int index = handle & 0xFF;

    if (type >= kNumResets || index >= MAX_RESETS_PER_TYPE)
        return kInvalidHandle;

    if (reset_handlers[type][index] == 0)
        return kNoHandler;

    reset_handlers[type][index] = 0;

    return kNoError;
}

/*
 * get_reset_type
 * Should be called soon after reset (i.e. early in main) and determines
 * the cause of the reset.  This function may be called only once per reset 
 * since it clears the associated RCON register bits.
 */

ResetType get_reset_type()
{
    ResetType type = kNumResets;

    if (_POR || _BOR)
        type = kPowerOnReset;
    else if (_SLEEP)
        type = kSleepReset;
    else if (_DPSLP)
        type = kDeepSleepReset;
    else if (_EXTR)
        type = kMCLRReset;
    else if (_SWR)
        type = kSoftwareReset;

    //Clear all the status flags so we don't get confused across
    //multiple resets
    _POR = 0;
    _SLEEP = 0;
    _DPSLP = 0;
    _EXTR = 0;
    _SWR = 0;

    return type;
}

void handle_reset()
{
    int i;
    ResetType type = get_reset_type();

    if (type >= kNumResets)
    {
        return;
    }

    //Call common functions
    for (i=0;i<MAX_RESETS_PER_TYPE;++i)
    {
        if (reset_handlers[kAllResetsBefore][i] != 0)
            reset_handlers[kAllResetsBefore][i](type);
    }

    //Call reset-type specific functions
    for (i=0;i<MAX_RESETS_PER_TYPE;++i)
    {
        if (reset_handlers[type][i] != 0)
            reset_handlers[type][i](type);
    }

    for (i=0;i<MAX_RESETS_PER_TYPE;++i)
    {
        if (reset_handlers[kAllResetsAfter][i] != 0)
            reset_handlers[kAllResetsAfter][i](type);
    }
}

void handle_all_resets_before(unsigned int type)
{
    configure_interrupts();
    taskloop_init();
    scheduler_init();

    //test scheduling code
    //scheduler_schedule_task(minutebeat, kEverySecond, 50, &minutetask);
    //scheduler_schedule_task(tenminutebeat, kEvery10Minutes, kScheduleForever, &tenminutetask);
    //scheduler_schedule_task(hourbeat, kEveryHour, kScheduleForever, &hourtask);
    //scheduler_schedule_task(daybeat, kEveryDay, kScheduleForever, &daytask);
}

void handle_all_resets_after(unsigned int type)
{
    //Add code that should be called after all other reset code here
}

void handle_poweron_reset(unsigned int type)
{
    //Power-on reset resets the rtcc, so configure and enable it.
    configure_rtcc();
    enable_rtcc();
}