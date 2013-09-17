#include "task_manager.h"
#include "common_types.h"
#include "uart.h"
#include "utilities.h"
#include "pic24.h"

taskmanager_data task_data;

//Internal Functions
static void taskman_push_runnable(task_info *task, int priority);

void taskman_init()
{
    int i;

    task_data.flags = 0;

    taskloop_set_sleep(0);

    //Initialize the list of free tasks
    taskman_init_task(&task_data.tasks[0]);

    task_data->free = &(task_data.tasks[0]);

    for (i=1; i<kMAXTASKS; ++i)
    {
        taskman_init_task(&task_data.tasks[i]);
        task_data.tasks[i].next = task_data->free;
        task_data->free = &task_data.tasks[i];
    }

    task_data.runnable_head = NULL;
    task_data.runnable_tail = NULL;
}

/*
 * Remove a taskinfo structure from the free list and return it.  
 * If the free list is empty, return NULL.  The removal of the task
 * from the free list is atomic.
 */

task_info* taskman_get_task()
{
    task_info *task;

    uninterruptible_start();

    if (task_data->free == NULL)
    {
        uninterruptible_end();
        return NULL;
    }

    task = task_data->free;
    task_data->free = task->next;

    uninterruptible_end();

    taskman_init_task(task);

    return task;
}

void taskman_init_task(task_info *task)
{
    task->callback = NULL;
    task->callback_arg = 0;
    task->next = NULL;
}

/*
 * INTERNAL FUNCTION
 * Given a task_info structure add it atomically to either the front or the back of the runnable
 * task list.  Cannot fail.
 */

static void taskman_push_runnable(task_info *task, int priority)
{
    uninterruptible_start();

    if (task_data.runnable_head == NULL)
    {
        //No runnable tasks
        task_data.runnable_tail = task;
        task_data.runnable_head = task;
        task_data.runnable_head->next = NULL;
    }
    else
    {
        if (priority)
        {
            task->next = task_data.runnable_head;
            task_data.runnable_head = task;
        }
        else
        {
            task->next = NULL;
            task_data.runnable_tail->next = task;
            task_data.runnable_tail = task;
        }
    }

    uninterruptible_end();
}

void taskman_run_task(task_callback function, int arg, int priority)
{
    task_info *task = taskman_get_task();

    if (task)
    {
        task->callback = function;
        task->callback_arg = arg;


        taskman_push_runnable(task, priority);
    }
    else
    {
        //FIXME: Log an error here, this should not happen in production b/c the taskman should have enough taskinfo structures
    }
}

void taskloop_set_sleep(int sleep)
{
    if (sleep)
        SET_BIT(taskqueue.flags, kTaskLoopSleepBit);
    else
        CLEAR_BIT(taskqueue.flags, kTaskLoopSleepBit);
}

void taskloop_loop()
{
    while(1)
    {
        while(taskloop_process_one())
            ;

        if (BIT_TEST(taskqueue.flags, kTaskLoopSleepBit))
            asm_sleep();

    }
}

int taskloop_process_one()
{
    task_callback task;

    if (ringbuffer_empty(&taskqueue.tasks))
        return 0;

    ringbuffer_pop(&taskqueue.tasks, &task);

    task();

    return 1;
}
