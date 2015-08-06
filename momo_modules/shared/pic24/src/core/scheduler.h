/*
 * scheduler.h
 * Module that uses the RTCC to schedule timed callbacks
 */

#ifndef __scheduler_h__
#define __scheduler_h__

#include "task_manager.h"
#include "rtcc.h"

#define kScheduleForever	0xFF

//Bits for the ScheduledTask flags
#define kScheduleForeverBit 		7
#define kBeingScheduledBit			6
#define kCallbackPendingBit			5

#define kScheduleFrequencyMask		0b111

#define task_frequency(task)		(task->flags & kScheduleFrequencyMask)

struct ScheduledTask_t
{
	task_callback 			callback;
	void*           		argument;
	
	unsigned char 			flags;
	unsigned char 			remaining_calls;

	int 					task_list;

	struct ScheduledTask_t 	*next;
};

typedef struct ScheduledTask_t ScheduledTask;


typedef struct
{
	//Task lists
	ScheduledTask *tasks[kNumAlarmTimes];

	AlarmRepeatTime call_frequency;
} SchedulerState;

//Scheduler Interface
void scheduler_init();

void scheduler_schedule_task( task_callback func,
                              AlarmRepeatTime freq,
                              unsigned char numtimes,
                              ScheduledTask /*out*/ *saved_task,
                              void *argument );

void scheduler_remove_task(ScheduledTask *task);

#endif