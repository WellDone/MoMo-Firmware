
#include "platform.h"

//Global State Types
typedef union 
{
	struct
	{
		volatile uint8 module_on:1;
		volatile uint8 shutdown_pending:1;
		volatile uint8 stream_in_progress:1;
		volatile uint8 stream_type:1;
		volatile uint8 unused:4;
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
#define RX_BUFFER_LENGTH 64
prefix uint8 gsm_rx_buffer[RX_BUFFER_LENGTH];
prefix uint8 rx_buffer_start;
prefix uint8 rx_buffer_end;
prefix uint8 rx_buffer_len;
prefix uint8 debug_val;

//GSM Module Status
prefix ModuleState state;
