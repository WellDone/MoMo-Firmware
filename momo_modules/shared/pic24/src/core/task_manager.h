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

typedef enum
{
    kTaskLoopSleepBit = 0,
    kTaskLoopLockedBit = 1,
    kTaskLoopLightSleepBit = 2      //Instruct the taskloop sleep handler that we expect to wake soon (used to keep the Flash memory powered up)
} TaskManagerFlag;

typedef enum
{
	kSleepCallback,
	kWakeupCallback
} TaskManagerCallbackReason;

typedef enum
{
    kCanEnterSleep = 0,
    kCannotEnterSleep
} TaskManagerSleepStatus;

//Callback Types
typedef void (*task_callback)(void);
typedef TaskManagerSleepStatus (*sleep_callback)(TaskManagerCallbackReason);

typedef struct
{
	task_callback callback;
	bool critical;
} task_item;

typedef struct
{
    task_item       taskdata[kMAXTASKS];
    ringbuffer      tasks;
    sleep_callback 	sleep_handler;

    unsigned int    flags;
} task_list;

//functions
void taskloop_init();


void taskloop_set_flag(TaskManagerFlag flag, unsigned int value);
int taskloop_get_flag(TaskManagerFlag flag);

void taskloop_set_sleephandler(sleep_callback handler);

int taskloop_add(task_callback task);
int taskloop_add_critical(task_callback task);

void taskloop_lock();
void taskloop_unlock();
bool taskloop_locked();

void taskloop_loop();
int taskloop_process_one();

#endif