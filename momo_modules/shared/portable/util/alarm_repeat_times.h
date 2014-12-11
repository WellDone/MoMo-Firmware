#ifndef __alarm_repeat_times_h__
#define __alarm_repeat_times_h__

typedef enum
{
    kEveryHalfSecond = 0,
    kEverySecond = 1,
    kEvery10Seconds = 2,
    kEveryMinute = 3,
    kEvery10Minutes = 4,
    kEveryHour = 5,
    kEveryDay = 6,
    kNumAlarmTimes
} AlarmRepeatTime;

#endif