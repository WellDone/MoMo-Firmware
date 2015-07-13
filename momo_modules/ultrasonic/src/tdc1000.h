#ifndef __tdc1000_h__
#define __tdc1000_h__

#include "communication.h"

typedef enum
{
	kFrequencyDiv2 		= 0,
	kFrequencyDiv4 		= 1,
	kFrequencyDiv8 		= 2,
	kFrequencyDiv16		= 3,
	kFrequencyDiv32 	= 4,
	kFrequencyDiv64 	= 5,
	kFrequencyDiv128	= 6,
	kFrequencyDiv256	= 7
} FrequencyDivisor;

#define kFrequencyDivisorMask	0b111
#define kNumTXMask				0b11111

typedef enum
{
	k1AverageCycle 		= 0,
	k2AverageCycles 	= 1,
	k4AverageCycles 	= 2,
	k8AverageCycles 	= 3,
	k16AverageCycles 	= 4,
	k32AverageCycles 	= 5,
	k64AverageCycles 	= 6,
	k128AverageCycles 	= 7
} NumberOfAverages;

#define kNumAveragesMask 0b111

typedef enum
{
	kDoNotCountEvents = 0,
	kReceive1Event = 1,
	kReceive2Events = 2,
	kReceive3Events = 3,
	kReceive4Events = 4,
	kReceive5Events = 5,
	kReceive6Events = 6,
	kReceive7Events = 7
} NumberExpectedReceiveEvents;

typedef enum
{
	kPGAGain0DB = 0,
	kPGAGain3DB,
	kPGAGain6DB,
	kPGAGain9DB,
	kPGAGain12DB,
	kPGAGain15DB,
	kPGAGain18DB,
	kPGAGain21DB
} PGAGainLevel;

typedef enum
{
	k35MVThreshold = 0,
	k50MVThreshold,
	k75MVThreshold,
	k125MVThreshold,
	k220MVThreshold,
	k410MVThreshold,
	k775MVThreshold,
	k1500MVThreshold
} EchoThreshold;

typedef enum
{
	kLNAEnabled = 0,
	kLNADisabled = 1
} LNAState;

typedef enum
{
	kTDC1000_LevelMode = 0,
	kTDC1000_TOFMode = 1,
	kTDC1000_MeterMode = 2
} MeasurementMode;

enum
{
	kTDC1000_Config0Reg = 0x00,
	kTDC1000_Config1Reg,
	kTDC1000_Config2Reg,
	kTDC1000_Config3Reg,
	kTDC1000_Config4Reg,
	kTDC1000_TOF_1Reg,
	kTDC1000_TOF_0Reg,
	kTDC1000_ErrorFlagsReg,
	kTDC1000_TimeoutReg,
	kTDC1000_ClockRateReg
};

//Register Structures
typedef struct
{
	union 
	{
		struct
		{
			uint8_t 	num_tx : 5;
			uint8_t 	tx_freq_div : 3;
		};

		uint8_t value;
	};
} tdc1000_config0;

typedef struct
{
	union
	{
		struct
		{
			uint8_t 	num_rx 	: 3;
			uint8_t		num_avg : 3;
			uint8_t		reserved: 2;
		};

		uint8_t value;
	};
} tdc1000_config1;

typedef struct
{
	union
	{
		struct
		{
			uint8_t 	tof_mode	: 2;
			uint8_t		chsel 		: 1;
			uint8_t		ext_chsel	: 1;
			uint8_t 	ch_swp		: 1;
			uint8_t		damping		: 1;
			uint8_t		meas_mode	: 1;
			uint8_t 	vcom_sel 	: 1;
		};

		uint8_t value;
	};
} tdc1000_config2;

typedef struct
{
	union
	{
		struct
		{
			uint8_t 	echo_threshold 	: 3;
			uint8_t		blanking 		: 1;
			uint8_t		temp_clk_div	: 1;
			uint8_t		temp_rtd_sel	: 1;
			uint8_t		temp_mode		: 1;
			uint8_t		reserved		: 1;
		};

		uint8_t value;
	};
} tdc1000_config3;

typedef struct
{
	union
	{
		struct
		{
			uint8_t 	tx_shift_pos 	: 5;
			uint8_t		trig_edge_pol 	: 1;
			uint8_t		receive_mode	: 1;
			uint8_t		reserved		: 1;
		};

		uint8_t value;
	};
} tdc1000_config4;

typedef struct
{
	union
	{
		struct
		{
			uint8_t 	timing_msb 	: 2;
			uint8_t		lna_fb 		: 1;
			uint8_t		lna_ctrl	: 1;
			uint8_t 	pga_ctrl	: 1;
			uint8_t		pga_gain	: 3;
		};

		uint8_t value;
	};
} tdc1000_tof1;

typedef struct
{
	union
	{
		uint8_t timing_lsb;
		uint8_t value;
	};

} tdc1000_tof0;

typedef struct
{
	union
	{
		struct
		{
			uint8_t 	sig_high	: 1;
			uint8_t		no_sig		: 1;
			uint8_t		sig_weak	: 1;
			uint8_t		reserved	: 5;
		};

		uint8_t value;
	};
} tdc1000_error;

typedef struct
{
	union
	{
		struct
		{
			uint8_t 	timeout_ctrl	: 2;
			uint8_t		echo_timeout	: 1;
			uint8_t		short_tof_blank	: 3;
			uint8_t		force_short_tof	: 1;
			uint8_t		reserved		: 1;
		};

		uint8_t value;
	};
} tdc1000_timeout;

typedef struct
{
	union
	{
		struct
		{
			uint8_t 	autozero_period	: 2;
			uint8_t		clockin_div		: 1;
			uint8_t		reserved		: 5;
		};

		uint8_t value;
	};
} tdc1000_clockrate;

//TDC1000 Configuration Structure
typedef struct
{
	tdc1000_config0		config0;
	tdc1000_config1		config1;
	tdc1000_config2 	config2;
	tdc1000_config3		config3;
	tdc1000_config4		config4;

	tdc1000_tof0		tof0;
	tdc1000_tof1		tof1;

	tdc1000_error		error;
	tdc1000_timeout		timeout;
	tdc1000_clockrate	clockrate;
} tdc1000_config;

void 			tdc1000_init();
uint8_t 		tdc1000_push();
tdc1000_error 	tdc1000_readerror();

void 			tdc1000_setgain(PGAGainLevel pga, LNAState lna, EchoThreshold threshold);
void			tdc1000_setmode(MeasurementMode mode);
void			tdc1000_setchannel(uint8_t channel);
void			tdc1000_prepare_deltatof(uint8_t channel_averages);
void 			tdc1000_setstarttime(uint16_t time);
void 			tdc1000_setexternal(uint8_t ext);

void 			tdc1000_setexcitation(uint8_t num_pulses, uint8_t num_expected);

#endif
