/* perf.h
 * Performance counting and profiling routines
 */

#ifndef __perf_h__
#define __perf_h__

#include "platform.h"

typedef enum
{
	kReadFlashMemory = 0,
	kWriteFlashMemory = 1,
	kWaitForFlashMemory = 2,
	kWaitForProgrammingCycle = 3,
	kWriteFlashBits = 4,
	kConstructReport,
	kNumPerformanceCounters
} PerformanceCounter;

typedef struct
{
	uint32	counters[kNumPerformanceCounters];
} profiler_data;

#define PROFILE_START(counter)		perf_start_profile(counter)
#define PROFILE_END(counter)		perf_end_profile(counter)

void 	perf_enable_profiling();
void 	perf_start_profile(PerformanceCounter counter);
void	perf_end_profile(PerformanceCounter counter);

uint32  perf_get_counter(PerformanceCounter counter);

#endif