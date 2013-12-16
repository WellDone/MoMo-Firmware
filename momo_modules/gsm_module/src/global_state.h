
#include "platform.h"

//Global State Types
typedef union 
{
	struct
	{
		volatile uint8 module_on;
		volatile uint8 unused:7;
	};

	volatile uint8 gsm_state;
} ModuleState;

#ifndef DEFINE_STATE
#define prefix extern
#else
#define prefix
#endif

//GSM Serial Communication Buffer
prefix uint8 gsm_buffer[32];
prefix uint8 buffer_len;

//GSM Module Status
prefix ModuleState state;
