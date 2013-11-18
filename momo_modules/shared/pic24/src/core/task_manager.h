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

typedef void (*task_callback)(int);

enum
{
    kTaskLoopSleepBit = 0,
    kTaskLoopLockedBit = 1
};

typedef struct task_struct
{
	task_callback 	callback;
	int 			callback_arg;

	struct task_struct *next;
} task_info;

typedef struct
{
    task_info tasks[kMAXTASKS];

    task_info *free;
    task_info *runnable;
    task_info *pending;
    int flags;

} taskmanager_data;

//functions
void taskloop_init();

void taskloop_set_sleep(int sleep);

int taskloop_add(task_callback task, int arg);

void taskloop_loop();
int taskloop_process_one();

#endif