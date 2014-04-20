#include "rtcc.h"
#include "pic24.h"
#include "task_manager.h"
#include <string.h>

alarm_callback the_alarm_callback = 0;
volatile uint16 alarm_time = kEveryHalfSecond;

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

void rtcc_get_timestamp(rtcc_timestamp* time)
{
    rtcc_datetime datetime;
    rtcc_get_time( &datetime );
    rtcc_create_timestamp( &datetime, time );
}
void rtcc_create_timestamp(const rtcc_datetime *source, rtcc_timestamp *dest)
{
    dest->year = source->year;
    dest->month = source->month;
    dest->day = source->day;
    dest->hours = source->hours;
    dest->minutes = source->minutes;
    dest->seconds = source->seconds;
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

bool isLeapYear( uint8 year )
{
    if ( ( (year%4 == 0) && (year%100 != 0) ) || (year%400 == 0) )
        return true;
    else
        return false;
}

int32 rtcc_timestamp_difference(rtcc_timestamp *time1, rtcc_timestamp *time2)
{
    int32 result = 0;
    uint8 i;
    result += time2->seconds - time1->seconds;
    result += 60 * (time2->minutes - time1->minutes);
    result += 3600L * (time2->hours - time1->hours);
    if ( time2->month == time1->month )
    {
        result += 86400L * (time2->day - time1->day );
    }
    else
    {
        uint8 start_month, end_month;
        int8 polarity;
        if ( time1->month < time2->month )
        {
            start_month = time1->month;
            end_month = time2->month;
            polarity = 1;
        }
        else
        {
            start_month = time2->month;
            end_month = time1->month;
            polarity = -1;
        }
        for ( i = start_month; i < end_month; ++i )
        {
            if ( i == 2 )
                result += polarity * 86400L * (isLeapYear( time1->year )? 28 : 29);
            else if ( i == 9 || i == 4 || i == 6 || i == 11 )
                result += polarity * 86400L * 30;
            else
                result += polarity * 86400L * 31;
        }
    }

    int8 year_delta = time2->year - time1->year;
    int8 polarity = (year_delta < 0)? -1 : 1;
    year_delta *= polarity;
    
    for ( i = 0; i < year_delta; ++i )
    {
        result += polarity * (isLeapYear( i )? 315360000L : 31449600);
    }

    return result;
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

    if (the_alarm_callback != 0)
        taskloop_add(the_alarm_callback);

    IFS3bits.RTCIF = 0;
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

void wait_ms( uint32 milliseconds )
{
    volatile uint32 tick = 0;
    milliseconds = milliseconds * CLOCKSPEED/1000;
    while ( tick!=milliseconds )
        ++tick;
}
