#ifndef __timers_h__
#define __timers_h__

enum
{
	kTimerAPrescale256 = 0b11,
	kTimerAPrescale64 = 0b10,
	kTimerAPrescale8 = 0b01,
	kTimerAPrescale1 = 0b00
};

enum
{
	kTimerASynchronize = 1,
	kTimerADontSynchronize = 0
};

enum
{
	kTimerALPRCSource = 0b10,
	kTimerAT1CKSource = 0b01,
	kTimerASoscSource = 0b00
};

enum
{
	kTimerASystemClock = 0,
	kTimerAExtendedClock = 1
};

enum
{
	kTimerADiscontinueIdle = 1,
	kTimerAContinueIdle = 0
};

//timer1 is a TimerA type timer
#define timer1_configure(extend, source, prescaler, idle)		T1CON = ((((idle) & 0b1) << 13) | (((source) & 0b11) << 8) | (((prescaler) & 0b11) << 4) | (((extend) & 0b1) << 1))
#define timer1_enable()											T1CONbits.TON = 1
#define timer1_disable()										T1CONbits.TON = 0
#define timer1_set(val)											TMR1 = (val)

#endif