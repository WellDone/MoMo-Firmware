/*
 * system_log - a lightweight activity log that supports arbitrary tagged data in the 
 * log entries.  Log entries are divided into fixed types to support automatic formatting
 * and display.  Each log entry starts with a LogEntryType entry that specifies the message
 * that the log entry should display.  This corresponds to an 8-byte MD5 hash for a log
 * message specified in a .ldf (Log Definition File) file that accompanies the source code
 * for the module.  These log definitions also define how to display the log message and 
 * how to format any accompanying data that might come along with the message.  
 * 
 * The initial entry is the followed by 0 or more data entries that define typed data
 * to accompany the log entry.  This allows easy programmatic access to the log information
 * and easy filtering based on message type, etc.  An arbitrary number of data entries can
 * accompany easy log entry.  In systems that contain external flash storage, the log is 
 * automatically persisted to the flash. In other systems it is stored in RAM only in a 
 * circular buffer.
 *
 * IMPORTANT NOTES:
 * - It is FORBIDDEN to call any of these routines from an interrupt.  They will break in
 	 sublte ways and this is bad practice anyway.  Interrupts should just set a flag for
 	 mainline code and queue a task.
 */

#ifndef __system_log_h__
#define __system_log_h__

/*
	Debug is the only mode.
*/

#include "platform.h"
#include "rtcc.h"
#include <stdarg.h>

//Memory Footprint Settings
#define LOG_BUFFER_SIZE 16

typedef enum 
{
	// 0 is reserved
	kCriticalLog 	= 1,
	kDebugLog    	= 2,
	kRemoteLog   	= 3, //initiated by an RPC
	kAuditLog	 	= 4,
	kPerformanceLog	= 5
} LogStream;

/*
 * There are 16 possible log entry types based on the size of the field
 */ 
typedef enum
{
	kNewLogEntryType = 0,
	kContinuationType = 1,
	kBlobType = 2,
	kIntegerListType = 3,
	kStringType = 4,
	kRemoteLogEndType = 5
} LogEntryType;

typedef struct
{
	uint8 type: 4;
	uint8 length: 4;
} LogEntryHeader;

//Generic log entry format without specifying the log entry contents
typedef struct
{
	LogEntryHeader 	header;
	uint8 			data[15];
} GenericLogEntry;

#define LOG_ENTRY_SIZE sizeof(GenericLogEntry)
#define LOG_DATA_SIZE  15

//LogEntry that defines the start of a new entry, possibly followed with
//tagged data entries.
typedef struct
{
	LogEntryHeader 	header; 	  	//1
	uint8 			stream;	  		//1
	
	rtcc_timestamp 	timestamp; 	  	//4
	uint32			message_hash; 	//4

	uint8 			remote_source;	//1
	uint8			remote_sequence;//1
	uint16			reserved[2];	//4
} TypedLogEntry;

//Initialization and Reading routines
void 	init_system_log(uint8 start_subsection, uint8 subsection_count);
bool 	read_system_log(uint16 offset, GenericLogEntry* out);
void 	disable_lazy_logging();
void 	clear_system_log();
void	flush_log(void*);
uint16 	system_log_count();

//Logging Interface
void log_start(LogStream stream, uint32 hash);
void log_integer(unsigned int value);
void log_array(LogEntryType type, const unsigned char *data, unsigned int length);
void log_string(const char *string);

#ifndef __TEST__
#define LOG_CRITICAL(msg) 		log_start(kCriticalLog, msg)
#define LOG_DEBUG(msg) 			log_start(kDebugLog, msg)
#define LOG_PERF(msg)			log_start(kPerformanceLog, msg)

#define LOG_INT(i)				log_integer(i)
#define LOG_POINTER(ptr)		log_integer((unsigned int)(ptr))
#define LOG_BOOL(b)				log_integer(b)
#define LOG_ARRAY(ptr, len)		log_array(kBlobType, (const unsigned char*)(ptr), len)	
#define LOG_STRING(str)			log_string(str)

#define LOG_FLUSH() 			flush_log(NULL)
#define DISABLE_LAZY_LOGGING()	disable_lazy_logging()

#else
#define LOG_CRITICAL(msg) 	
#define LOG_DEBUG(msg) 
#define LOG_PERF(msg)		

#define LOG_INT(i)			
#define LOG_POINTER(ptr)	
#define LOG_BOOL(b)			
#define LOG_ARRAY(ptr, len)	
#define LOG_STRING(str)

#define LOG_FLUSH() 	
#define DISABLE_LAZY_LOGGING()	
#endif

#endif