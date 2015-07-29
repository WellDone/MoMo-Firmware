#ifndef __report_comm_stream_h__
#define __report_comm_stream_h__

#include <stdint.h>
#include "task_manager.h"

typedef enum
{
	kCSIdleState = 0,
	kCSStartStreamingState = 1,
	kCSSetDestination = 2,
	kCSSetAPN = 3,
	kCSOpenStream = 4,
	kCSPushToStream = 5,
	kCSCloseStream = 6,
	kCSFinishStreamingState = 7,
	kCSErrorOccurredState
} ReportStreamingState;

typedef enum
{
	kCSNoError = 0,
	kCSStreamAlreadyInProgress,
	kCSInvalidCommStreamStateObject
} CommStreamError;

typedef union
{
	struct
	{
		uint8_t reserved: 8;
	};

	uint8_t value;
} CommStreamFlags;

typedef struct
{
	ReportStreamingState 	state;
	
	int 				 	last_error;

	CommStreamFlags			flags;
	uint8_t					address;

	const char *			report;
	unsigned int 			report_length;

	unsigned int 			scratch_counter;

	task_callback 			callback;
} CommStreamState;

void commstream_init(CommStreamState *state);
void commstream_setcallback(CommStreamState *info, task_callback callback);

CommStreamError commstream_start(CommStreamState *state, uint8_t address, const char *report, unsigned int length);
CommStreamError commstream_abort(CommStreamState *state);

#endif