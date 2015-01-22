#include "reset_manager.h"
#include "pic24.h"

//Global reset handler table
reset_handler reset_handlers[kNumResets][MAX_RESETS_PER_TYPE] =
{
    {0}, //POR Reset
    {0}, //Reset during sleep
    {0}, //Wake from deep sleep
    {0}, //Software reset
    {0}, //MCLR External reset
    {0}, //Call on all resets (before)
    {0}  //Call after all other reset code
};

ResetType last_reset;

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

    if (_EXTR)
        type = kMCLRReset;
    else if (_SWR)
        type = kSoftwareReset;
    else if (_POR || _BOR)
        type = kPowerOnReset;

    //Clear all the status flags so we don't get confused across
    //multiple resets
    _POR = 0;
    _SLEEP = 0;
    _DPSLP = 0;
    _EXTR = 0;
    _SWR = 0;

    return type;
}

ResetType last_reset_type()
{
    return last_reset;
}

void handle_reset()
{
    int i;
    ResetType type = get_reset_type();
    last_reset = type;

    //Call common functions
    for (i=0;i<MAX_RESETS_PER_TYPE;++i)
    {
        if (reset_handlers[kAllResetsBefore][i] != 0)
            reset_handlers[kAllResetsBefore][i](type);
    }

    if (type < kNumResets)
    {
        //Call reset-type specific functions
        for (i=0;i<MAX_RESETS_PER_TYPE;++i)
        {
            if (reset_handlers[type][i] != 0)
                reset_handlers[type][i](type);
        }
    }

    //Call post reset functions
    for (i=0;i<MAX_RESETS_PER_TYPE;++i)
    {
        if (reset_handlers[kAllResetsAfter][i] != 0)
            reset_handlers[kAllResetsAfter][i](type);
    }
}
