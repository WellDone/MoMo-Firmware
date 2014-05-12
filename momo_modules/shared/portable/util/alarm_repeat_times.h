#ifndef __alarm_repeat_times_h__
#define __alarm_repeat_times_h__

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

#endif