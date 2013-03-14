//tasks.c

#include "tasks.h"
#include "common.h"
#include "serial.h"

task_list taskqueue;

void taskloop_init()
{
    ringbuffer_create(&taskqueue.tasks, (void*)taskqueue.taskdata, sizeof(task_callback), kMAXTASKS);
    taskqueue.flags = 0;
}

void taskloop_set_sleep(int sleep)
{
    if (sleep)
        SET_BIT(taskqueue.flags, kTaskLoopSleepBit);
    else
        CLEAR_BIT(taskqueue.flags, kTaskLoopSleepBit);
}

int taskloop_add(task_callback task)
{
    if (ringbuffer_full(&taskqueue.tasks))
        return 0;

    ringbuffer_push(&taskqueue.tasks, (void *)task);

    return 1;
}

void taskloop_loop()
{
    while(1)
    {
        print( "processing one task\r\n");
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