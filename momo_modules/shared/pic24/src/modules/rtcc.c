#include "rtcc.h"
#include "pic24.h"
#include "task_manager.h"
#include <string.h>

alarm_callback the_alarm_callback = 0;
volatile uint16 alarm_time = kEveryHalfSecond;
static int callback_pending = 0;

static void rtcc_callback(void *unused);

const uint32    year_seconds                    = 24LL*60LL*60LL*365LL;
const uint16    halfday_seconds                 = 12LL*60LL*60LL;
const uint32    day_seconds                     = 24LL*60LL*60LL;
const uint32    month_lintable[kNumMonths]      = {0LL, 2678400LL, 5097600LL, 7776000LL, 10368000LL, 13046400LL, 15638400LL, 18316800LL, 20995200LL, 23587200LL, 26265600LL, 28857600LL};

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

uint16 rtcc_enabled()
{
    return _RTCEN;
}

void configure_rtcc()
{
    rtcc_datetime date;
    _RTCCMD = 0; //Make sure power to the rtcc is enabled.

    if (!_RTCWREN)
        asm_enable_rtcon_write();

    _RTCLK = 0b01;
    _CAL = 0; //Clear oscillator trimming
    _RTCOE = 0; //Don't output the clock signal
    _ALRMEN = 0; //Don't set an alarm
    _CHIME = 0; //Don't chime

    date.year = 0;
    date.month = 1;
    date.day = 1;
    date.hours = 0;
    date.minutes = 0;
    date.seconds = 0;
    date.weekday = 0;

    rtcc_set_time(&date);
}

/*
 * Get the current time from the rtcc module
 * There's a possibility of a race condition during reading though
 * so check to make sure the registers didn't update midread and recover
 * if that happened
 */

void rtcc_set_time(rtcc_datetime *time)
{
    uint16 old_status = rtcc_enabled();

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

rtcc_timestamp rtcc_get_timestamp()
{
    rtcc_datetime datetime;
    rtcc_get_time(&datetime);
    return rtcc_create_timestamp(&datetime);
}

rtcc_timestamp rtcc_create_timestamp(const rtcc_datetime *source)
{
    rtcc_timestamp out = 0;

    //Add in the year without accounting for leap years
    out += year_seconds*source->year;

    //Add in one extra day for each leap year between 2000 and source->year
    //Remember that 2000 was a leap year
    if (source->year > 0)
        out += (((source->year-1)>>2) + 1)*source->year;

    //Add in the number of seconds in this year to the month
    if (source->month > 0)
        out += month_lintable[source->month];

    //If we're currently in a leap year and after February, add in an extra day
    //This simplified rule 
    if (((source->year%4) == 0) && (source->month > 2))
        out += day_seconds;

    if (source->day > 0)
        out += (source->day-1)*day_seconds;

    out += source->hours * 3600;
    out += source->minutes * 60;
    out += source->seconds;

    return out;
}

uint16 rtcc_datetimes_equal(rtcc_datetime *time1, rtcc_datetime *time2)
{
    return (rtcc_compare_times(time1, time2) == 0);
}

/*
 * rtcc_compare_times
 * Return <0 if time1 is sooner than time2
 * Return 0  if time1 == time2
 * Return >0 if time1 is after time2
 */
uint16 rtcc_compare_times(rtcc_datetime *time1, rtcc_datetime *time2)
{
    return memcmp(time1, time2, kTimeCompareSize);
}

/* 
 * Return the absolute number of seconds between time1 and time2.  If time2 > time1, direction will be set
 * to kPositiveDelta.  If time2 < time1, direction will be set to kNegativeDelta.  If time1 == time2, direction
 * will be set to kZeroDelta.  Note that an int32 will potentially overflow, which is why the sign bit is returned
 * separately.
 */

uint32 rtcc_timestamp_difference(rtcc_timestamp time1, rtcc_timestamp time2, TimeIntervalDirection *direction)
{
    TimeIntervalDirection   dir  = kZeroDelta;  
    uint32                  diff = 0;

    if (time1 > time2)
    {
        dir = kNegativeDelta;
        diff = time1 - time2;
    }
    else if (time1 < time2)
    {
        dir = kPositiveDelta;
        diff = time2 - time1;
    }

    if (direction)
            *direction = dir;

    return diff;
}

void get_rtcc_datetime_unsafe(rtcc_datetime *time)
{
    uint16 curr;

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
    uint16 curr;

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

uint8 from_bcd(uint8 val)
{
    return ((val&0xF0) >> 4)*10 + (val&0x0F);
}

uint8 to_bcd(uint8 val)
{
    return ((val/10) << 4) | (val%10);
}

void __attribute__((interrupt,no_auto_psv)) _RTCCInterrupt()
{
    uint16 curr_t, curr_a;
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

    if (the_alarm_callback != 0 && !callback_pending)
    {
        callback_pending = 1;
        taskloop_add(rtcc_callback, NULL);
    }

    IFS3bits.RTCIF = 0;
}

static void rtcc_callback(void *arg)
{
    if (the_alarm_callback)
        the_alarm_callback(arg);

    callback_pending = 0;
}

uint16 last_alarm_frequency()
{
    return alarm_time;
}

void set_recurring_task(AlarmRepeatTime repeat, alarm_callback routine)
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

    _ALRMEN = 1; //Enable the recurring task

    //Save off the callback
    the_alarm_callback = routine;

    IPC15bits.RTCIP = 1;
    IEC3bits.RTCIE = 1;
}

void clear_recurring_task()
{
    if (!_RTCWREN)
        asm_enable_rtcon_write();

    uninterruptible_start();
    _ALRMEN = 0; //disable alarm
    the_alarm_callback = 0;
    uninterruptible_end();
}