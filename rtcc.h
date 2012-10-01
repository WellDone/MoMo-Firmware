
#ifndef __rtcc_h__
#define __rtcc_h__

#include "interrupts.h"

#define HIBYTE(x) ((x&0xFF00) >> 8)
#define LOBYTE(x) (x&0xFF)
#define PACKWORD(hi, lo) ((hi&0xFF) << 8 | (lo&0xFF))

//Type Definitions
typedef void (*alarm_handler)(void);
typedef enum
{
    EveryHalfSecond = 0b0000,
    EverySecond = 0b0001,
    Every10Seconds = 0b0010,
    EveryMinute = 0b0011,
    Every10Minutes = 0b0100,
    EveryHour = 0b0101,
    EveryDay = 0b0110
} AlarmRepeatTime;

typedef struct
{
    unsigned char seconds;
    unsigned char minutes;
    unsigned char hours;

    unsigned char day;
    unsigned char weekday;
    unsigned char month;
    unsigned char year;
} rtcc_time;

//Assembly function prototypes
extern void asm_enable_rtcon_write();

//rtcc module function prototypes
void enable_rtcc();
void disable_rtcc();

unsigned int rtcc_enabled();

void configure_rtcc();
void configure_rtcc_oscillator();

unsigned int rtcc_times_equal(rtcc_time *time1, rtcc_time *time2);

void rtcc_get_time(rtcc_time *time);
void rtcc_set_time(rtcc_time *time);

//Utility functions
void get_rtcc_time_unsafe(rtcc_time *time);
unsigned char from_bcd(unsigned char val);
unsigned char to_bcd(unsigned char val);
void set_recurring_task(AlarmRepeatTime repeat, alarm_handler routine);

#define RTCC_ISR_DESCRIPTOR (isr_descriptor){ \
                                0b100,        \
                                0,            \
                                (unsigned int*)IEC3,         \
                                (unsigned int*)IPC15,        \
                                (unsigned int*)IFS3,         \
                                14,           \
                                8,            \
                                14}

#endif
