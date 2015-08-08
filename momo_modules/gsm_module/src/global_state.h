#ifndef __global_state_h__
#define __global_state_h__

#include "platform.h"

//Global State Types
typedef union 
{
	struct
	{
		volatile uint8_t stream_in_progress:1;
		volatile uint8_t stream_type:1;
		volatile uint8_t unused:6;
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

typedef enum
{
	kNoGSMError = 0,
	kSIMNotDetected = 1,
	kModuleCouldNotTurnOn = 2,
	kModuleCouldNotDisableEchoing = 3,
	kModuleCouldNotSetErrorFormat = 4,
	kCouldNotSetSMSFormat = 5,
	kModuleNotOnWhenExpected = 6,
	kGSMTimeoutWaitingForNetwork = 7,
	kGPRSTimeoutAwaitingCGREG = 8,
	kGPRSErrorSettingBearerType = 9,
	kGPRSErrorSettingAPN = 10,
	kGPRSTimeoutWaitingBearer = 11,
	kGPRSErrorDisconnecting = 12,
	kStreamNotInProgress = 13,
	kStreamAlreadyInProgress = 14,
	kHTTPCouldNotEnableSSL = 15,
	kHTTPCouldNotInitialize = 16,
	kHTTPTimeoutPreparingWrite = 17,
	kHTTPCouldNotSetCID = 18,
	kHTTPCouldNotSetContentType = 19,
	kHTTPCouldNotSetURL = 20,
	kHTTPCouldNotSetRedirection = 21,
	kHTTPCouldNotPostData = 22,
	kHTTPTimeoutWaitingForPost = 23,
	kHTTPFailureCodeReturned = 24
} GSMError;

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

#endif