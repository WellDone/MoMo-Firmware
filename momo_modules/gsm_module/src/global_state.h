
#include "platform.h"

//Global State Types
typedef union 
{
	struct
	{
		volatile uint8_t module_on:1;
		volatile uint8_t callback_pending:1;
		volatile uint8_t stream_in_progress:1;
		volatile uint8_t stream_type:1;
		volatile uint8_t stream_success:1;
		volatile uint8_t unused:3;
	};

	volatile uint8_t gsm_state;
} ModuleState;

typedef enum
{
	kModuleOff = 0,
	kModuleOnNotConnected = 1,
	kModuleOnConnected = 2,
	kModuleOnDataOn = 3
} ModuleInfo;

typedef enum
{
	kNoRequestedTask = 0,
	kEnableModule,
	kDisableModule,
	kSendCommand,
	kConnectToNetwork,
	kConnectToGPRS,
	kOpenStream,
	kPushToStream,
	kCloseStream
} RequestedTask;

#define kStreamSMS 0
#define kStreamGPRS 1

#ifndef DEFINE_STATE
#define prefix extern
#else
#define prefix
#endif

//GSM Serial Communication Receive Buffer
prefix uint8_t debug_val;

//GSM Module Status
prefix ModuleState 				state;
prefix ModuleInfo				connection_status;
prefix volatile RequestedTask 	rpc_request;
prefix volatile uint8_t 		async_length;