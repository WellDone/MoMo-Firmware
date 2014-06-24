
#include "perf.h"
#include <stdint.h>
#include <xc.h>

profiler_data	profiler;

void perf_enable_profiling()
{
	T2CONbits.TSIDL = 0;
	T2CONbits.TGATE = 0;
	T2CONbits.TCKPS = 0b00; //1:1 prescaling
	T2CONbits.T32 = 1;
	T2CONbits.TCS = 0;

	TMR2 = 0;
	TMR3 = 0;

	T2CONbits.TON = 1;
}

void perf_start_profile(PerformanceCounter counter)
{
	if (counter < kNumPerformanceCounters)
		profiler.counters[counter] = (((uint32)TMR3) << 16) | TMR2;
}

void perf_end_profile(PerformanceCounter counter)
{
	uint32 diff = (((uint32)TMR3) << 16) | TMR2;
	if (counter < kNumPerformanceCounters)
	{
		if (diff > profiler.counters[counter])
			diff -= profiler.counters[counter];
		else
			diff = 0xFFFFFFFFULL - diff + profiler.counters[counter];

		profiler.counters[counter] = diff;
	}
}

uint32 perf_get_counter(PerformanceCounter counter)
{
	return profiler.counters[counter];
}