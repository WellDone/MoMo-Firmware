#ifndef __tdc7200_h__
#define __tdc7200_h__

#include "communication.h"

enum
{
	kTDC7200_Config1Reg = 0x00,
	kTDC7200_Config2Reg = 1,
	kTDC7200_INTStatusReg = 2,
	kTDC7200_INTMaskReg = 3,
	kTDC7200_CoarseOverflowHigh = 4,
	kTDC7200_CoarseOverflowLow = 5,
	kTDC7200_ClockOverflowHigh = 6,
	kTDC7200_ClockOverflowLow = 7,
	kTDC7200_ClockStopHigh = 8,
	kTDC7200_ClockStopLow = 9,
	kTDC7200_Time1 = 0x10,			//Note the gap in numbering here
	kTDC7200_Clock1 = 0x11,
	kTDC7200_Time2 = 0x12,
	kTDC7200_Clock2 = 0x13,
	kTDC7200_Time3 = 0x14,
	kTDC7200_Clock3 = 0x15,
	kTDC7200_Time4 = 0x16,
	kTDC7200_Clock4 =0x17,
	kTDC7200_Time5 = 0x18,
	kTDC7200_Clock5 = 0x19,
	kTDC7200_Time6 = 0x1A,
	kTDC7200_Calibration1 = 0x1B,
	kTDC7200_Calibration2 = 0x1C
};

typedef struct
{
	union
	{
		struct
		{
			uint8_t start_meas		: 1;
			uint8_t meas_mode		: 2;
			uint8_t start_edge		: 1;
			uint8_t stop_edge		: 1;
			uint8_t	trigg_edge		: 1;
			uint8_t parity_enable	: 1;
			uint8_t force_cal		: 1;
		};

		uint8_t value;
	};
} tdc7200_config1;

typedef struct
{
	union
	{
		struct
		{
			uint8_t num_stops		: 3;
			uint8_t avg_cycles		: 3;
			uint8_t cal_periods		: 2;
		};

		uint8_t value;
	};
} tdc7200_config2;

typedef struct
{
	union
	{
		struct
		{
			uint8_t new_meas_int		: 1;
			uint8_t coarse_overflow_int	: 1;
			uint8_t clock_overflow_int	: 1;
			uint8_t meas_started_int	: 1;
			uint8_t meas_complete_int	: 1;
			uint8_t reserved			: 3;
		};

		uint8_t value;
	};
} tdc7200_intstatus;

typedef struct
{
	union
	{
		struct
		{
			uint8_t new_meas_mask		: 1;
			uint8_t coarse_overflow_mask: 1;
			uint8_t clock_overflow_mask	: 1;
			uint8_t reserved			: 5;
		};

		uint8_t value;
	};
} tdc7200_intmask;

typedef struct
{
	tdc7200_config1 config1;
	tdc7200_config2 config2;
	tdc7200_intmask intmask;
} tdc7200_config;

void 		tdc7200_init();
uint8_t 	tdc7200_start();

//Data Fetching
uint32_t 	tdc7200_calibration();
int32_t 	tdc7200_tof(uint8_t index);

void 		tdc7200_setstops(uint8_t stops);

#endif
