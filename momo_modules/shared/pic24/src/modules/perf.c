
#include "perf.h"
#include <stdint.h>
#include <xc.h>

#ifndef __NO_PERF__

profiler_data	profiler;

void perf_enable_profiling()
{
	int i;

	T2CONbits.TSIDL = 0;
	T2CONbits.TGATE = 0;
	T2CONbits.TCKPS = 0b00; //1:1 prescaling
	T2CONbits.T32 = 1;
	T2CONbits.TCS = 0;

	TMR2 = 0;
	TMR3 = 0;

	T2CONbits.TON = 1;

	for (i=0; i<kNumPerformanceCounters; ++i)
		perf_init_counter((PerformanceCounter)i);
}

void perf_start_profile(PerformanceCounter counter)
{
	if (counter < kNumPerformanceCounters)
		profiler.counters[counter].accum = (((uint32)TMR3) << 16) | TMR2;
}

void perf_end_profile(PerformanceCounter counter)
{
	uint32 diff = (((uint32)TMR3) << 16) | TMR2;
	
	if (diff > profiler.counters[counter].accum)
		diff -= profiler.counters[counter].accum;
	else
		diff = 0xFFFFFFFFULL - diff + profiler.counters[counter].accum;

	perf_update_counter(counter, diff);
}

void perf_init_counter(PerformanceCounter counter_index)
{
	performance_counter *c;

	if (counter_index >= kNumPerformanceCounters)
		return;

	c = &profiler.counters[counter_index];

	c->min = 0xFFFFFFFFULL;
	c->max = 0;
	c->mean = 0;
	c->count = 0;
	c->accum = 0;
}

void perf_update_counter(PerformanceCounter counter_index, uint32 value)
{
	performance_counter *c;

	if (counter_index >= kNumPerformanceCounters)
		return;

	c = &profiler.counters[counter_index];
	++c->count;

	if (value < c->min)
		c->min = value;

	if (value > c->max)
		c->max = value;

	c->mean = (c->mean*7 + value) >> 3;
}

uint32 perf_get_mean(PerformanceCounter counter)
{
	return profiler.counters[counter].mean;
}

uint32 perf_get_max(PerformanceCounter counter)
{
	return profiler.counters[counter].max;
}

uint32 perf_get_min(PerformanceCounter counter)
{
	return profiler.counters[counter].min;
}

uint32 perf_get_count(PerformanceCounter counter)
{
	return profiler.counters[counter].count;
}

const performance_counter * perf_get_counter(PerformanceCounter counter)
{
	return &profiler.counters[counter];
}

#endif
