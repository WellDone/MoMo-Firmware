#include "task_manager.h"
#include "common_types.h"
#include "uart.h"
#include "utilities.h"
#include "pic24.h"
#include "memory.h"
#include "bus_master.h"
#include "system_log.h"
#include "timers.h"
#include "scheduler.h"

#ifndef __TEST__
#include "log_definitions.h"
#endif

task_list       taskqueue;
unsigned int    sleep_time;
unsigned int    num_notsleeps;
ScheduledTask   perf_task;

static void taskloop_log_performance(void *arg);


void taskloop_init()
{
    ringbuffer_create(&taskqueue.tasks, (void*)taskqueue.taskdata, sizeof(task_item), kMAXTASKS);
    taskqueue.flags = 0;
    taskqueue.sleep_handler = NULL;

    taskloop_set_flag(kTaskLoopSleepBit, 0);

    /*
     * Initialize a counter to tell what fraction of the time we're asleep so that we can compute
     * power usage statistics.  We keep a free running timer on timer1 powered from the RTCC 32khz
     * clock and a separate counter saying how often we're asleep.
     */

     timer1_configure(kTimerAExtendedClock, kTimerALPRCSource, kTimerAPrescale8, kTimerAContinueIdle);
     timer1_set(0);
     timer1_enable();
     sleep_time = 0;
     num_notsleeps = 0;

     scheduler_schedule_task(taskloop_log_performance, kEvery10Seconds, kScheduleForever, &perf_task, NULL);
}

void taskloop_log_performance(void *arg)
{
    if (sleep_time < kMINSLEEPTICKS && TMR1 >= kMINSLEEPTICKS)
    {
        int i;

        LOG_PERF(kSleepTimeMessage);
        LOG_INT(TMR1);
        LOG_INT(sleep_time);
        LOG_INT(num_notsleeps);

        //Log all of the tasks in the queue so that we can see who is stopping us
        //from sleeping
        for (i=0; i<taskqueue.tasks.count; ++i)
            LOG_ARRAY(ringbuffer_peekindex(&taskqueue.tasks, i), sizeof(task_item));
    }

    timer1_set(0);
    sleep_time = 0;
    num_notsleeps = 0;
}

void taskloop_set_flag(unsigned int flag, unsigned int value)
{
    if (flag >= 16)
        return;

    if (value)
        SET_BIT(taskqueue.flags, flag);
    else
        CLEAR_BIT(taskqueue.flags, flag);
}

int taskloop_get_flag(unsigned int flag)
{
    if (flag >= 16)
        return 0;

    return BIT_TEST(taskqueue.flags, flag);
}

void taskloop_set_sleephandler(sleep_callback handler)
{
    taskqueue.sleep_handler = handler;
}

int taskloop_add_impl(task_callback task, void* argument, bool critical )
{
    task_item object;
    object.callback = task;
    object.argument = argument;
    object.critical = critical;

    //If the taskloop is full, choke out a message and reset to a hopefully better future.
    if ( ringbuffer_full( &taskqueue.tasks ) )
    {
        //Make sure nothing can interrupt us, this is the end...
        disable_interrupts();

        //Reset the PIC since this is an unrecoverable error.
        DISABLE_LAZY_LOGGING(); //disable lazing logging since that requires a working taskloop
        LOG_CRITICAL(kTaskLoopFullError);
        while(!ringbuffer_empty(&taskqueue.tasks))
        {
            task_item task;
            ringbuffer_pop(&taskqueue.tasks, &task);
            LOG_ARRAY(&task, sizeof(task_item));
        }

        asm_reset();
    }

    ringbuffer_push( &taskqueue.tasks, &object );

    return 1;
}

int taskloop_add(task_callback task, void* argument)
{
    return taskloop_add_impl( task, argument, false );
}

int taskloop_add_critical(task_callback task, void* argument)
{
    return taskloop_add_impl( task, argument, true );   
}

void taskloop_lock()
{
    LOG_CRITICAL(kTaskLoopLockedNotice);
    LOG_FLUSH();
    SET_BIT(taskqueue.flags, kTaskLoopLockedBit);
}
void taskloop_unlock()
{
    if ( taskloop_locked() )
    {
        LOG_CRITICAL(kTaskLoopUnlockedNotice);
        LOG_FLUSH();
    }
    CLEAR_BIT(taskqueue.flags, kTaskLoopLockedBit);
}
bool taskloop_locked()
{
    return BIT_TEST(taskqueue.flags, kTaskLoopLockedBit);
}

void taskloop_loop()
{
    while(1)
    {
        while (taskloop_process_one())
            ;

        if (BIT_TEST(taskqueue.flags, kTaskLoopSleepBit))
        {
            if (taskqueue.sleep_handler == NULL || taskqueue.sleep_handler(kSleepCallback) == kCanEnterSleep)
            {
                unsigned int sleep_start = TMR1;
                asm_sleep();
                sleep_time += TMR1 - sleep_start;
            }
            else
                ++num_notsleeps;
        }
    }
}
int taskloop_process_one()
{
    task_item task;

    if (ringbuffer_empty(&taskqueue.tasks))
        return 0;

    ringbuffer_pop(&taskqueue.tasks, &task);
    if ( taskloop_locked() && !task.critical )
        ringbuffer_push( &taskqueue.tasks, &task );
    else
        task.callback( task.argument );

    return 1;
}
