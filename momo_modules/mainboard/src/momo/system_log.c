#include "system_log.h"
#include "flash_queue.h"
#include "ringbuffer.h"
#include "task_manager.h"
#include <string.h>

#define LOG_BUFFER_SIZE 10
#define LOG_ENTRY_SIZE 63
typedef struct
{
	uint8 length;
	BYTE data[LOG_ENTRY_SIZE];
} LogEntry;

static flash_queue log_queue;
static ringbuffer log_buffer;
static LogEntry log_buffer_data[LOG_BUFFER_SIZE];

static LogEntry staged_log_entry;

static bool flush_task_pending = false;

void init_system_log( uint8 start_subsection, uint8 subsection_count )
{
	ringbuffer_create( &log_buffer, log_buffer_data, sizeof( LogEntry ), LOG_BUFFER_SIZE );
  flash_queue_create( &log_queue, start_subsection,
                      sizeof(LogEntry), 
                      subsection_count );
}
void flush_task( void* arg )
{
	flush_task_pending = false;
	while ( !ringbuffer_empty( &log_buffer ) )
	{
		flash_queue_queue( &log_queue, ringbuffer_peek( &log_buffer ) );
		ringbuffer_pop( &log_buffer, NULL );
	}
}
void write_system_log( const char* data, uint8 length )
{
	uninterruptible_start();

	if ( length > LOG_ENTRY_SIZE )
		return;

	if ( ringbuffer_full( &log_buffer ) )
		flush_task( NULL ); // This will lock things up but we need to make sure we save off the log entries

	staged_log_entry.length = length;
	// TODO: Can we copy this directly to the ringbuffer instead of doing an additional memcpy to stage?
	memcpy( &staged_log_entry.data, data, length );

	// NB: There will be garbage after the end of the data buffer, but it's not worth zeroing out

	ringbuffer_push( &log_buffer, &staged_log_entry );

	if ( !flush_task_pending )
	{
		flush_task_pending = true;
		taskloop_add( flush_task, NULL );
	}
	uninterruptible_end();
}