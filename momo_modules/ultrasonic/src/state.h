#ifndef __state_h__
#define __state_h__

#include <stdint.h>

typedef union
{
	struct
	{
		uint8_t test_level_measurement			: 1;
		uint8_t take_delta_tof_measurement		: 1;
		uint8_t find_pulse_variance				: 1;
		uint8_t find_variance					: 1;
		uint8_t find_optimal_gain 				: 1;
		uint8_t take_tof_measurement			: 1;
		uint8_t find_signal_strength			: 1;
		uint8_t reserved						: 1;
	};

	uint8_t value;
} async_flags;

typedef enum
{
	kAutomaticMode = 0,
	kManualMode
} MeasurementMode;

typedef enum
{
	kNotInAutomaticMode = 0,
	kSearchingForSignal = 1,
	kAcquiringData = 2
} AutomaticModeStatus;

typedef struct
{
	volatile async_flags		flags;
	volatile MeasurementMode 	mode;

	AutomaticModeStatus			autostatus;


} module_state;

#endif