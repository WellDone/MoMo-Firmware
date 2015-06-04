#include "oscillator.h"
#include "pic24.h"
#include "utilities.h"

void oscillator_init()
{
    set_oscillator_speed(k8MhzFRC, false);
    set_sosc_status(1); //Secondary oscillator always needs to be enabled, disable sosc
}

int set_oscillator_speed(FRCPostscaler speed, int use_pll)
{
    unsigned char oscconh;
    unsigned char oscconl;

    //You cannot enable the PLL if the FRC is not between 4 and 8 Mhz
    if (use_pll && speed > k4MhzFRC)
        return false;

    uninterruptible_start();

    oscconh = OSCCONH;
    oscconl = OSCCONL;

    //select new speed
    oscconh &= ~(0b111);

    if (use_pll)
        oscconh |= kFRCDIVPLL;
    else
        oscconh |= kFRCDIV;

    __builtin_write_OSCCONH(oscconh);

    _RCDIV = speed;

    SET_BIT(oscconl, 0);
    __builtin_write_OSCCONL(oscconl);

    //Wait for the switch to happen
    while (_OSWEN)
        ;

    //If we're using the PLL, wait for it to stabilize
    if (use_pll)
    {
        while (_LOCK == 0)
            ;
    }

    uninterruptible_end();

    return true;
}

/*
 * pass enabled=1 to enable the secondary oscillator
 * pass enabled=0 to disable the secondary oscillator
 * STATUS: Tested and working (Tim Burke) 3/11/13
 */
void set_sosc_status(int enabled)
{
    unsigned char oscl = OSCCONL;

    if (enabled > 0)
        SET_BIT(oscl, 1); //SOSCEN is bit 1
    else
        CLEAR_BIT(oscl, 1); //Clear bit

    __builtin_write_OSCCONL(oscl);
}

/*
 * get_sosc_status
 * returns 1 is the secondary oscillator is enabled and 0 otherwise
 * STATUS: Tested and working (Tim Burke) 3/11/13
 */
int get_sosc_status()
{
    return (OSCCONL & (1<<1)) >> 1;
}
