#ifndef __system_log_h__
#define __system_log_h__

/*
	Debug is the only mode.
*/

#include "platform.h"
#include "rtcc.h"

#define LOG_ENTRY_SIZE 56

typedef enum {
	// 0 is reserved
	kCriticalLog = 1,
	kDebugLog    = 2,
	kRemoteLog   = 3 //initiated by an RPC
} LogStream;

typedef struct
{
	LogStream stream;
	uint8 length;
	rtcc_timestamp timestamp; //6
	BYTE data[LOG_ENTRY_SIZE];
} LogEntry;

void init_system_log( uint8 start_subsection, uint8 subsection_count );
void write_system_log( LogStream stream, const BYTE* data, uint8 length );
bool read_system_log( uint16 offset, LogEntry* out );
void clear_system_log();
uint16 system_log_count();

#define CRITICAL_LOG( data, length ) write_system_log( kDebugLog, (BYTE*)data, length )
#define CRITICAL_LOGL( literal ) CRITICAL_LOG( literal, sizeof(literal) )

#define DEBUG_LOG( data, length ) write_system_log( kDebugLog, (BYTE*)data, length )
#define DEBUG_LOGL( literal ) DEBUG_LOG( literal, sizeof(literal) )

#endif