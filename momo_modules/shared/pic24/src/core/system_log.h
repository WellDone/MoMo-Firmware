#ifndef __system_log_h__
#define __system_log_h__

/*
	Debug is the only mode.
*/

#include "platform.h"
#include "rtcc.h"
#include <stdarg.h>

#define LOG_ENTRY_SIZE 56

typedef enum {
	// 0 is reserved
	kCriticalLog = 1,
	kDebugLog    = 2,
	kRemoteLog   = 3 //initiated by an RPC
} LogStream;

typedef struct
{
	uint8 stream; // LogStream
	uint8 length;
	rtcc_timestamp timestamp; //4
	uint16 reserved;
	BYTE data[LOG_ENTRY_SIZE];
} LogEntry;

#ifndef __NO_FLASH__

void init_system_log( uint8 start_subsection, uint8 subsection_count );
void write_system_log( LogStream stream, const BYTE* data, uint8 length );
void write_system_logf( LogStream stream, const char* fmt, ... );
bool read_system_log( uint16 offset, LogEntry* out );
void disable_lazy_logging();
void clear_system_log();
uint16 system_log_count();

void flush_log(void*);

#define CRITICAL_LOG( data, length ) write_system_log( kCriticalLog, (BYTE*)data, length )
#define CRITICAL_LOGL( literal ) CRITICAL_LOG( literal, sizeof(literal) )

#define DEBUG_LOG( data, length ) write_system_log( kDebugLog, (BYTE*)data, length )
#define DEBUG_LOGL( literal ) DEBUG_LOG( literal, sizeof(literal) )

#define FLUSH_LOG() flush_log(NULL)

#else
#define CRITICAL_LOG( data, length ) 
#define CRITICAL_LOGL( literal ) CRITICAL_LOG( literal, sizeof(literal) )
#define DEBUG_LOG( data, length ) 
#define DEBUG_LOGL( literal ) DEBUG_LOG( literal, sizeof(literal) )
#define FLUSH_LOG() 
#define disable_lazy_logging()	

#endif
#endif