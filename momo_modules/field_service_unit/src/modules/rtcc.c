#include "rtcc.h"
#include "common.h"
#include <string.h>

task_callback alarm_callback = 0;
volatile unsigned int alarm_time = kEveryHalfSecond;

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
}

/*
 * Get the current time from the rtcc module
 * There's a possibility of a race condition during reading though
 * so check to make sure the registers didn't update midread and recover
 * if that happened
 */

void rtcc_set_time(rtcc_datetime *time)
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

void rtcc_get_time(rtcc_datetime *time)
{
    rtcc_datetime first_read;

    get_rtcc_datetime_unsafe(&first_read);
    get_rtcc_datetime_unsafe(time);

    if (rtcc_datetimes_equal(&first_read, time))
        return;

    get_rtcc_datetime_unsafe(time);
}

unsigned int rtcc_datetimes_equal(rtcc_datetime *time1, rtcc_datetime *time2)
{
    return (rtcc_compare_times(time1, time2) == 0);
}

/*
 * rtcc_compare_times
 * Return <0 if time1 is sooner than time2
 * Return 0  if time1 == time2
 * Return >0 if time1 is after time2
 */
unsigned int rtcc_compare_times(rtcc_datetime *time1, rtcc_datetime *time2)
{
    return memcmp(time1, time2, kTimeCompareSize);
}

void rtcc_datetime_difference(rtcc_datetime *time1, rtcc_datetime *time2)
{
    time2->year     -= time1->year;
    time2->month    -= time1->month;
    time2->day      -= time1->day;
    time2->weekday  -= time1->weekday;
    time2->minutes  -= time1->minutes;
    time2->seconds  -= time1->seconds;
}

void get_rtcc_datetime_unsafe(rtcc_datetime *time)
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

void rtcc_get_alarm(rtcc_datetime *alarm)
{
    unsigned int curr;

    _ALRMPTR = 0b10;

    curr = ALRMVAL;
    alarm->month = from_bcd(HIBYTE(curr));
    alarm->day = from_bcd(LOBYTE(curr));

    curr = ALRMVAL;
    alarm->weekday = from_bcd(HIBYTE(curr));
    alarm->hours = from_bcd(LOBYTE(curr));

    curr = ALRMVAL;
    alarm->minutes = from_bcd(HIBYTE(curr));
    alarm->seconds = from_bcd(LOBYTE(curr));
}

unsigned char from_bcd(unsigned char val)
{
    return ((val&0xF0) >> 4)*10 + (val&0x0F);
}

unsigned char to_bcd(unsigned char val)
{
    return ((val/10) << 4) | (val%10);
}

void __attribute__((interrupt,no_auto_psv)) _RTCCInterrupt()
{
    unsigned int curr_t, curr_a;
    rtcc_datetime diff;


    //Check what interval we were called at
    //compare in bcd format so that nibbles compare as ones and tens digits
    _RTCPTR  = 0b10;
    _ALRMPTR = 0b10;

    curr_t = RTCVAL;
    curr_a = ALRMVAL;
    diff.month = (HIBYTE(curr_t)^HIBYTE(curr_a));
    diff.day = (LOBYTE(curr_t)^LOBYTE(curr_a));

    curr_t = RTCVAL;
    curr_a = ALRMVAL;
    diff.weekday = (HIBYTE(curr_t)^HIBYTE(curr_a));
    diff.hours = (LOBYTE(curr_t)^LOBYTE(curr_a));

    curr_t = RTCVAL;
    curr_a = ALRMVAL;
    diff.minutes = (HIBYTE(curr_t)^HIBYTE(curr_a));
    diff.seconds = (LOBYTE(curr_t)^LOBYTE(curr_a));

    //Distinguish half second and second intervals by a special bit
    alarm_time = kEveryHalfSecond;

    if (_HALFSEC == 0)
        alarm_time = kEverySecond;

    if ((diff.seconds & 0x0F) == 0)
    {
        //at least 10 second interval
        if (diff.seconds == 0)
        {
            //at least minute interval
            if ((diff.minutes & 0x0F) == 0)
            {
                //at least 10 minutes
                if (diff.minutes == 0)
                {
                    //at least 1 hour
                    if (diff.hours == 0)
                        alarm_time = kEveryDay;
                    else
                        alarm_time = kEveryHour;
                }
                else
                    alarm_time = kEvery10Minutes;
            }
            else
                alarm_time = kEveryMinute;
        }
        else
            alarm_time = kEvery10Seconds;
    }

    if (alarm_callback != 0)
        taskloop_add(alarm_callback);

    IFS3bits.RTCIF = 0;
}

unsigned int last_alarm_frequency()
{
    return alarm_time;
}

void set_recurring_task(AlarmRepeatTime repeat, task_callback routine)
{
    if (!_RTCWREN)
        asm_enable_rtcon_write();

    _ALRMEN = 0; //disable alarm
    _CHIME = 1; //allow infinite repeats

    _ALRMPTR = 0b10;
    ALRMVAL = PACKWORD(to_bcd(1), to_bcd(1)); //Set the alarm to trigger at 00:00 1/1
    ALRMVAL = PACKWORD(to_bcd(0), to_bcd(0)); //day of week, hour
    ALRMVAL = PACKWORD(to_bcd(0), to_bcd(0)); //minute, second

    _ARPT = 0x01; //Repeat once but since chime is on, we'll roll over, so repeat forever

    //Set the time between recurrences.
    _AMASK = repeat;

    //Save off the callback
    alarm_callback = routine;

    IPC15bits.RTCIP = 1;
    IEC3bits.RTCIE = 1;
    _ALRMEN = 1; //Enable the recurring task
}

void clear_recurring_task()
{
    if (!_RTCWREN)
        asm_enable_rtcon_write();

    uninterruptible_start();
    _ALRMEN = 0; //disable alarm
    alarm_callback = 0;
    uninterruptible_end();
}

/*void wait( unsigned int milliseconds )
{
    isr_descriptor desc = RTCC_ISR_DESCRIPTOR;
    if ( !_RTCWREN)
        asm_enable_rtcon_write();

    _ALRMEN = 0;
    _CHIME = 0;

    _ALRMPTR = 0b10;
    ALRMVAL = PACKWORD(to_bcd(1), to_bcd(1));

    _ARPT = 0x01;
    _AMASK = 0x00;
}*/

void wait_ms( unsigned long milliseconds )
{
    volatile unsigned long tick = 0;
    milliseconds = milliseconds * CLOCKSPEED/1000;
    while ( tick!=milliseconds )
        ++tick;
}
