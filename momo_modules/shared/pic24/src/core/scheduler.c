#include "scheduler.h"
#include "common_types.h"
#include "utilities.h"
#include "pic24.h"
#include "system_log.h"

static SchedulerState state;

//Internal functions
static void scheduler_update_rate();
static void scheduler_list_insert(ScheduledTask **list, ScheduledTask *task);
static void scheduler_list_remove(ScheduledTask **list, ScheduledTask *task);
static void scheduler_callback();
static void scheduler_call_task(void *task);

void scheduler_init()
{
	int i;

	state.call_frequency = kNumAlarmTimes;

	for (i =0; i<kNumAlarmTimes; ++i)
		state.tasks[i] = 0;
}

void scheduler_schedule_task( task_callback func,
                              AlarmRepeatTime freq,
                              unsigned char numtimes,
                              ScheduledTask /*out*/ *saved_task,
                              void *argument )
{
	//If this task is already scheduled, remove it and then readd it so we don't get corruptions
	if (BIT_TEST(saved_task->flags, kBeingScheduledBit))
	{
		unsigned int old_freq = task_frequency(saved_task);

		//Sanity check, ensure the old frequency makes sense
		if (old_freq < kNumAlarmTimes)
			scheduler_list_remove(&state.tasks[old_freq], saved_task);
	}

	saved_task->callback = func;
	saved_task->argument = argument;
	saved_task->flags = 0;
	saved_task->remaining_calls = numtimes;
	saved_task->flags |= (freq & kScheduleFrequencyMask);

	if (numtimes == kScheduleForever)
		SET_BIT(saved_task->flags, kScheduleForeverBit);

	scheduler_list_insert(&state.tasks[freq], saved_task);
}

void scheduler_remove_task(ScheduledTask *task)
{
	int list = task->flags & kScheduleFrequencyMask;

	scheduler_list_remove(&state.tasks[list], task);
}

//Internal functions
/*
 * Should be called in an uninterruptible section (most likely)
 */
void scheduler_update_rate()
{
	int i;

	//Find the first occupied time slot
	for (i=0; i<kNumAlarmTimes; ++i)
	{
		if (state.tasks[i] != 0)
			break;
	}

	//If the state has changed update the alarm repetition rate
	if (state.call_frequency != i)
	{
		state.call_frequency = i;

		if (i == kNumAlarmTimes)
			clear_recurring_task();
		else
			set_recurring_task((AlarmRepeatTime)i, scheduler_callback);
	}

}

void scheduler_list_insert(ScheduledTask **list, ScheduledTask *task)
{
	uninterruptible_start();

	if (*list == 0)
	{
		*list = task;
		task->next = 0;
	}
	else
	{
		task->next = *list;
		*list = task;
	}

	SET_BIT(task->flags, kBeingScheduledBit);

	scheduler_update_rate();
	uninterruptible_end();
}

void scheduler_list_remove(ScheduledTask **list, ScheduledTask *task)
{
	ScheduledTask *curr = *list;

	if (*list == 0)
		return;

	uninterruptible_start();

	if (*list == task)
	{
		*list = task->next;
		task->next = 0;
	}
	else
	{
		while (curr != 0)
		{
			if (curr->next == task)
			{
				curr->next = task->next;
				task->next = 0;
				break;
			}

			curr = curr->next;
		}
	}

	CLEAR_BIT(task->flags, kBeingScheduledBit);

	scheduler_update_rate();
	uninterruptible_end();
}

static void scheduler_callback()
{
	unsigned int max = last_alarm_frequency();
	unsigned int i;
	ScheduledTask *curr;

	for (i=0; i<= max; ++i)
	{
		for(curr=state.tasks[i]; curr; curr = curr->next)
		{
			if (BIT_TEST(curr->flags, kCallbackPendingBit))
			{
				CRITICAL_LOGL("Scheduled callback period came and went without callback being called.");
				continue;
			}

			SET_BIT(curr->flags, kCallbackPendingBit);
			curr->task_list = i; //we need to know the task list number in the callback below if we have to remove this task
			taskloop_add(scheduler_call_task, curr);
		}
	}
}

static void scheduler_call_task(void *task)
{
	ScheduledTask *curr = task;

	if ( !BIT_TEST(curr->flags, kBeingScheduledBit) ) // Sanity check, in case the task was removed after this instance was queued
		return;

	//Call the function and then let it be rescheduled for callback
	//if needed.  This is so if something is locking the taskloop
	//for a long time we don't add the same callback N times in a row
	//without it ever being called.  Call it immediately since we
	//were added to the task loop just to call it.
	curr->callback(curr->argument);
	CLEAR_BIT(curr->flags, kCallbackPendingBit);

	if (BIT_TEST(curr->flags, kScheduleForeverBit) == 0)
		--curr->remaining_calls;
	
	if (curr->remaining_calls == 0)
		scheduler_list_remove(&state.tasks[curr->task_list], curr);
}