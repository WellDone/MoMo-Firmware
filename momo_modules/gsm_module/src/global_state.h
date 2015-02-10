
#include "platform.h"

//Global State Types
typedef union 
{
	struct
	{
		volatile uint8 module_on:1;
		volatile uint8 callback_pending:1;
		volatile uint8 stream_in_progress:1;
		volatile uint8 stream_type:1;
		volatile uint8 stream_success:1;
		volatile uint8 unused:3;
	};

	volatile uint8 gsm_state;
} ModuleState;

#define kStreamSMS 0
#define kStreamGPRS 1

#ifndef DEFINE_STATE
#define prefix extern
#else
#define prefix
#endif

//GSM Serial Communication Receive Buffer
prefix uint8 debug_val;

//GSM Module Status
prefix ModuleState state;
