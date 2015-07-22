#ifndef __report_comm_stream_h__
#define __report_comm_stream_h__

#include <stdint.h>

void 	report_stream_abandon();
int 	report_stream_send(char* buffer);

void notify_report_success();
void notify_report_failure();

void init_comm_stream();

typedef enum
{
	kIdleState = 0,
	kStartStreamingState,
	kSetDestination,
	kSetAPN,
	kOpenStream,
	kPushToStream,
	kCloseStream,
	kErrorOccurredState
} ReportStreamingState;

typedef struct
{
	ReportStreamingState 	state;
	
	int 				 	last_error;	
	uint8_t					address;
	uint8_t					streaming_allowed;
} CommStreamState;

#endif