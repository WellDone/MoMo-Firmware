#include "system_log.h"
#include "flash_queue.h"
#include "ringbuffer.h"
#include "task_manager.h"
#include <string.h>

#define LOG_BUFFER_SIZE 8

static flash_queue log_queue;
static ringbuffer log_buffer;
LogEntry log_buffer_data[LOG_BUFFER_SIZE];

static bool flush_task_pending = false;
bool lazy_system_logging = true;

void init_system_log( uint8 start_subsection, uint8 subsection_count )
{
	ringbuffer_create( &log_buffer, log_buffer_data, sizeof( LogEntry ), LOG_BUFFER_SIZE );
  flash_queue_create( &log_queue, start_subsection,
                      sizeof(LogEntry), 
                      subsection_count );
}

void disable_lazy_logging()
{
	lazy_system_logging = 0;
}

void flush_log( void* arg )
{
	flush_task_pending = false;
	while ( !ringbuffer_empty( &log_buffer ) )
	{
		flash_queue_queue( &log_queue, ringbuffer_peek( &log_buffer ) );
		ringbuffer_pop( &log_buffer, NULL );
	}
}
void write_system_log( LogStream stream, const BYTE* data, uint8 length )
{
	uninterruptible_start();

	if ( length > LOG_ENTRY_SIZE )
		length = LOG_ENTRY_SIZE;

	if ( ringbuffer_full( &log_buffer ) )
		flush_log( NULL ); // This will lock things up but we need to make sure we save off the log entries

	LogEntry* staged_log_entry = (LogEntry*) ringbuffer_stage( &log_buffer );
	staged_log_entry->stream = stream;
	staged_log_entry->length = length;
	staged_log_entry->timestamp = rtcc_get_timestamp();
	memcpy( &staged_log_entry->data, data, length );
	ringbuffer_commit( &log_buffer );

	// NB: There will be garbage after the end of the data buffer, but it's not worth zeroing out

	if ( lazy_system_logging && !flush_task_pending )
	{
		flush_task_pending = true;
		taskloop_add( flush_log, NULL );
	}
	else if ( !lazy_system_logging )
	{
		flush_log( NULL );
	}
	uninterruptible_end();
}

bool read_system_log( uint16 index, LogEntry *out )
{
	flash_queue_walker walker = new_flash_queue_walker( &log_queue, index );
	if ( flash_queue_walk( &walker, (void*)out, 1 ) == 0 )
		return false;
	else
		return true;
}
void clear_system_log()
{
	flash_queue_reset( &log_queue );
}

uint16 system_log_count()
{
	return flash_queue_count( &log_queue );
}