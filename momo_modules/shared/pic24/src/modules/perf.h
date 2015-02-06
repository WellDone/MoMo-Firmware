/* perf.h
 * Performance counting and profiling routines
 */

#ifndef __perf_h__
#define __perf_h__

#include "platform.h"

#ifndef __NO_PERF__

typedef enum
{
	kReadFlashMemory = 0,
	kWriteFlashMemory = 1,
	kWaitForFlashMemory = 2,
	kWaitForProgrammingCycle = 3,
	kWriteFlashBits = 4,
	kConstructReport = 5,
	kEncodeReportCounter = 6,
	kProcessEventCounter = 7,
	kNumPerformanceCounters
} PerformanceCounter;

typedef struct
{
	uint32 count;
	uint32 mean; //compute an exponential filter with size 8 
	uint32 min;
	uint32 max;

	uint32 accum;
} performance_counter;

typedef struct
{
	performance_counter	counters[kNumPerformanceCounters];
} profiler_data;

#define PROFILE_START(counter)		perf_start_profile(counter)
#define PROFILE_END(counter)		perf_end_profile(counter)

//Initialization function must be called before profiling anything
void 	perf_enable_profiling();
void 	perf_init_counter(PerformanceCounter counter_index);

//Profiling API
void 	perf_start_profile(PerformanceCounter counter);
void	perf_end_profile(PerformanceCounter counter);

//Internal use and manual updating of performance data
void	perf_update_counter(PerformanceCounter, uint32 value);

//Routines for getting performance data
uint32  perf_get_count(PerformanceCounter counter);
uint32  perf_get_mean(PerformanceCounter counter);
uint32  perf_get_min(PerformanceCounter counter);
uint32  perf_get_max(PerformanceCounter counter);

const performance_counter * perf_get_counter(PerformanceCounter counter);

#else
#define PROFILE_START(counter)		
#define PROFILE_END(counter)		

#endif

#endif