//scheduler.c

#include "scheduler.h"
#include "core/common.h"

static SchedulerState state;

//Internal functions
static void scheduler_update_rate();
static void scheduler_list_insert(ScheduledTask **list, ScheduledTask *task);
static void scheduler_list_remove(ScheduledTask **list, ScheduledTask *task);
static void scheduler_callback();

void scheduler_init()
{
	int i;

	state.call_frequency = kNumAlarmTimes;

	for (i =0; i<kNumAlarmTimes; ++i)
		state.tasks[i] = 0;
}

void scheduler_schedule_task(task_callback func, AlarmRepeatTime freq, unsigned char numtimes, ScheduledTask /*out*/ *saved_task)
{
	saved_task->callback = func;
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
	uninterruptible_start();

	if (*list == 0)
		return;

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

void scheduler_callback()
{
	unsigned int max = last_alarm_frequency();
	unsigned int i;
	ScheduledTask *curr, *last;

	for (i=0; i<= max; ++i)
	{
		curr = state.tasks[i];

		while (curr)
		{
			if (BIT_TEST(curr->flags, kScheduleForeverBit) == 0)
			{
				--curr->remaining_calls;
			}

			taskloop_add(curr->callback);

			last = curr;
			curr = curr->next;

			if (last->remaining_calls == 0)
				scheduler_list_remove(&state.tasks[i], last);
		}
	}
}