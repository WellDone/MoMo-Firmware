#include "rtcc.h"
#include <p24F16KA101.h>

alarm_handler alarm_isr = 0;

void enable_rtcc()
{
    if (!_RTCWREN)
        asm_enable_rtcon_write();

    _RTCEN = 1;
    
}

void disable_rtcc()
{
    if (!_RTCWREN)
        asm_enable_rtcon_write();

    _RTCEN = 0;
}

unsigned int rtcc_enabled()
{
    return _RTCEN;
}

void configure_rtcc()
{
    _RTCCMD = 0; //Make sure power to the rtcc is enabled.

    if (!_RTCWREN)
        asm_enable_rtcon_write();

    _CAL = 0; //Clear oscillator trimming
    _RTCOE = 0; //Don't output the clock signal
    _ALRMEN = 0; //Don't set an alarm
    _CHIME = 0; //Don't chime

    configure_rtcc_oscillator();
}

/*
 * The RTCC requires a secondary lowspeed oscillator at 32.768 khz
 *
 */
void configure_rtcc_oscillator()
{
    _SOSCEN = 1;

    unsigned int externalOscillator = 0b100; // << 12?
    __builtin_write_OSCCONH( externalOscillator );
    __builtin_write_OSCCONL( externalOscillator );
    _OSWEN = 1;
    //Perhaps additional oscillator configuration?
}

/*
 * Get the current time from the rtcc module
 * There's a possibility of a race condition during reading though
 * so check to make sure the registers didn't update midread and recover
 * if that happened
 */

void rtcc_set_time(rtcc_time *time)
{
    unsigned int old_status = rtcc_enabled();

    if (!_RTCWREN)
        asm_enable_rtcon_write();

    disable_rtcc();

    _RTCPTR = 0b11;

    RTCVAL = PACKWORD(0, to_bcd(time->year));
    RTCVAL = PACKWORD(to_bcd(time->month), to_bcd(time->day));
    RTCVAL = PACKWORD(to_bcd(time->weekday), to_bcd(time->hours));
    RTCVAL = PACKWORD(to_bcd(time->minutes), to_bcd(time->seconds));

    if (old_status)
        enable_rtcc();
}

void rtcc_get_time(rtcc_time *time)
{
    rtcc_time first_read;

    get_rtcc_time_unsafe(&first_read);
    get_rtcc_time_unsafe(time);

    if (rtcc_times_equal(&first_read, time))
        return;

    get_rtcc_time_unsafe(time);
}

unsigned int rtcc_times_equal(rtcc_time *time1, rtcc_time *time2)
{
    return (memcmp(time1, time2, sizeof(rtcc_time))==0);
}

void get_rtcc_time_unsafe(rtcc_time *time)
{
    unsigned int curr;

    _RTCPTR = 0b11; //Load in the

    curr = RTCVAL;
    time->year = from_bcd(LOBYTE(curr));

    curr = RTCVAL;
    time->month = from_bcd(HIBYTE(curr));
    time->day = from_bcd(LOBYTE(curr));

    curr = RTCVAL;
    time->weekday = from_bcd(HIBYTE(curr));
    time->hours = from_bcd(LOBYTE(curr));

    curr = RTCVAL;
    time->minutes = from_bcd(HIBYTE(curr));
    time->seconds = from_bcd(LOBYTE(curr));
}

unsigned char from_bcd(unsigned char val)
{
    return ((val&0xF0) >> 4)*10 + (val&0x0F);
}

unsigned char to_bcd(unsigned char val)
{
    return ((val/10) << 4) | (val%10);
}

void __attribute__((interrupt,no_auto_psv)) handle_alarm()
{    
    if (alarm_isr != 0)
        alarm_isr();

    //FIXME clear isf
}

void set_recurring_task(AlarmRepeatTime repeat, alarm_handler routine)
{
    isr_descriptor desc = RTCC_ISR_DESCRIPTOR;
     if (!_RTCWREN)
        asm_enable_rtcon_write();

     _ALRMEN = 0; //disable alarm
     _CHIME = 1; //allow infinite repeats

     _ALRMPTR = 0b10;
     ALRMVAL = PACKWORD(to_bcd(1), to_bcd(1)); //Set the alarm to trigger at 00:00 1/1
     ALRMVAL = PACKWORD(to_bcd(0), to_bcd(0));
     ALRMVAL = PACKWORD(to_bcd(0), to_bcd(0));

     _ARPT = 0x01; //Repeat once but since chime is on, we'll roll over, so repeat forever

     //Set the time between recurrences.
     _AMASK = repeat;

     //load the ISR
     alarm_isr = routine;
     //install_isr(&desc); FIXME: fix this

     _ALRMEN = 1; //Enable the recurring task
}

void wait( unsigned int milliseconds )
{
    isr_descriptor desc = RTCC_ISR_DESCRIPTOR;
    if ( !_RTCWREN)
        asm_enable_rtcon_write();

    _ALRMEN = 0;
    _CHIME = 0;

    _ALRMPTR = 0b10;
    ALRMVAL = PACKWORD(to_bcd(1), to_bcd(1));

    _ARPT = 0x01;
    _AMASK =
}