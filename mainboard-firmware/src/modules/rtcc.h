
#ifndef __rtcc_h__
#define __rtcc_h__

#include "interrupts.h"
#include "task_manager.h"

#define HIBYTE(x) ((x&0xFF00) >> 8)
#define LOBYTE(x) (x&0xFF)
#define PACKWORD(hi, lo) ((hi&0xFF) << 8 | (lo&0xFF))

//Type Definitions
typedef enum
{
    kEveryHalfSecond = 0b0000,
    kEverySecond = 0b0001,
    kEvery10Seconds = 0b0010,
    kEveryMinute = 0b0011,
    kEvery10Minutes = 0b0100,
    kEveryHour = 0b0101,
    kEveryDay = 0b0110,
    kNumAlarmTimes
} AlarmRepeatTime;

//Only the first 6 bytes of rtcc_time are significant for comparisons.  Don't compare the weekday
#define kTimeCompareSize    6

typedef struct
{
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hours;
    unsigned char minutes;
    unsigned char seconds;

    unsigned char weekday;
    unsigned char unused;
} rtcc_time;

//Assembly function prototypes
extern void asm_enable_rtcon_write();

//rtcc module function prototypes
void enable_rtcc();
void disable_rtcc();

unsigned int rtcc_enabled();

void configure_rtcc();

unsigned int    rtcc_times_equal(rtcc_time *time1, rtcc_time *time2);
unsigned int    rtcc_compare_times(rtcc_time *time1, rtcc_time *time2);
void            rtcc_time_difference(rtcc_time *time1, rtcc_time *time2); //overwrite time2 with time2-time1 componentwise;

void rtcc_get_time(rtcc_time *time);
void rtcc_set_time(rtcc_time *time);
void rtcc_get_alarm(rtcc_time *alarm);

//Utility functions
void get_rtcc_time_unsafe(rtcc_time *time);
unsigned char from_bcd(unsigned char val);
unsigned char to_bcd(unsigned char val);

void         set_recurring_task(AlarmRepeatTime repeat, task_callback routine);
void         clear_recurring_task();
unsigned int last_alarm_frequency();

#endif
