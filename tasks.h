/*
 * tasks.h
 * A cooperative multitasking loop.
 * Anyone can add callback functions to the loop and the loop will
 * execute any callbacks in a fixed size ringbuffer and then either idle
 * or sleep until there are more tasks.
 */

#ifndef __tasks_h__
#define __tasks_h__

#include "ringbuffer.h"

#define kMAXTASKS 16 //NB Must be a power of 2 since it will be used for a ringbuffer

typedef void (*task_callback)(void);

typedef enum
{
    kTaskLoopSleepBit = 0
};

typedef struct
{
    task_callback taskdata[kMAXTASKS];
    ringbuffer tasks;
    
    unsigned int flags;
} task_list;

//functions
void taskloop_init();

void taskloop_set_sleep(int sleep);

int taskloop_add(task_callback task);

void taskloop_loop();
int taskloop_process_one();

#endif