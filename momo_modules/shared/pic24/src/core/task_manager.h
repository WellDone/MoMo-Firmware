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
#include "rtcc.h"

#define kMAXTASKS 16 //NB Must be a power of 2 since it will be used for a ringbuffer

typedef void (*task_callback)(void);

enum
{
    kTaskLoopSleepBit = 0,
    kTaskLoopLockedBit = 1
};

typedef struct
{
	task_callback callback;
	bool critical;
} task_item;

typedef struct
{
    task_item taskdata[kMAXTASKS];
    ringbuffer tasks;

    unsigned int flags;
} task_list;

//functions
void taskloop_init();

void taskloop_set_sleep(int sleep);

int taskloop_add(task_callback task);
int taskloop_add_critical(task_callback task);

void taskloop_lock();
void taskloop_unlock();
bool taskloop_locked();

void taskloop_loop();
int taskloop_process_one();

#endif