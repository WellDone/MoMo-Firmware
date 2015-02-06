
#ifndef __rtcc_h__
#define __rtcc_h__

#include "interrupts.h"
#include "common_types.h"
#include "alarm_repeat_times.h"

#define HIBYTE(x) ((x&0xFF00) >> 8)
#define LOBYTE(x) (x&0xFF)
#define PACKWORD(hi, lo) ((hi&0xFF) << 8 | (lo&0xFF))

typedef void(*alarm_callback)(void*);

//Constants for converting between rtcc_datetime and rtcc_timestamp efficiently
enum
{
    kNumMonths = 12
};

typedef enum
{
    kRTCCSoscSource = 0,
    kRTCCLPRCSource = 1      //NB, on the 24FJ64GA306 the LPRC is +- 20% accuracy, use with caution!
} RTCCClockSource;

typedef enum
{
    kPositiveDelta = 1,
    kZeroDelta = 0,
    kNegativeDelta = -1
} TimeIntervalDirection;

//Only the first 6 bytes of rtcc_datetime are significant for comparisons.  Don't compare the weekday
#define kTimeCompareSize    6

//rtcc_timestamp stores the number of elapsed seconds between 12:00AM January 1, 2000 and the given time
//It is valid for 1 century (until 2100)
typedef uint32 rtcc_timestamp;

typedef struct
{
    uint8 year;
    uint8 month;
    uint8 day;
    uint8 hours;
    uint8 minutes;
    uint8 seconds;

    uint8 weekday;
    uint8 unused;
} rtcc_datetime;

typedef struct
{
    uint8 year;
    uint8 month;
    uint8 day;
} rtcc_date;

typedef struct
{
    uint8 hour;
    uint8 minute;
    uint8 second;
} rtcc_time;

//Assembly function prototypes
extern void asm_enable_rtcon_write();

//rtcc module function prototypes
void enable_rtcc();
void disable_rtcc();

unsigned int rtcc_enabled();

void configure_rtcc(RTCCClockSource source);

unsigned int    rtcc_datetimes_equal(rtcc_datetime *time1, rtcc_datetime *time2);
unsigned int    rtcc_compare_times(rtcc_datetime *time1, rtcc_datetime *time2);
uint32          rtcc_timestamp_difference(rtcc_timestamp time1, rtcc_timestamp time2, TimeIntervalDirection *direction);

void rtcc_get_time(rtcc_datetime *time);
void rtcc_set_time(rtcc_datetime *time);
void rtcc_get_alarm(rtcc_datetime *alarm);

rtcc_timestamp rtcc_get_timestamp();
rtcc_timestamp rtcc_create_timestamp(const rtcc_datetime *source);

//Utility functions
void get_rtcc_datetime_unsafe(rtcc_datetime *time);
uint8 from_bcd(uint8 val);
uint8 to_bcd(uint8 val);

void         set_recurring_task(AlarmRepeatTime repeat, alarm_callback routine);
void         clear_recurring_task();
unsigned int last_alarm_frequency();

#endif
