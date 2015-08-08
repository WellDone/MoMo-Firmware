#include "system_log.h"
#include "flashqueue2.h"
#include "ringbuffer.h"
#include "task_manager.h"
#include "utilities.h"
#include <string.h>

#define kSystemLogVersion	1

//Global variables for the system_log module
#ifndef __NO_FLASH__
static fq_data log_queue;
#endif

static ringbuffer log_buffer;

static GenericLogEntry log_buffer_data[LOG_BUFFER_SIZE];

static bool flush_task_pending = false;
static bool lazy_system_logging = true;

//Private functions
static void ensure_log_space();
static void commit_log_entry();

void init_system_log(uint8 start_subsection, uint8 subsection_count)
{
	ringbuffer_create(&log_buffer, log_buffer_data, sizeof(GenericLogEntry), LOG_BUFFER_SIZE);

	#ifndef __NO_FLASH__
	fq_init(&log_queue, kSystemLogVersion, start_subsection, sizeof(GenericLogEntry), subsection_count);
	#endif
}

void disable_lazy_logging()
{
	lazy_system_logging = 0;
}

void log_start(LogStream stream, uint32 hash)
{
	ensure_log_space();

	TypedLogEntry entry;
	entry.stream = stream;
	entry.message_hash = hash;
	entry.timestamp = rtcc_get_timestamp();

	entry.header.type = kNewLogEntryType;
	entry.header.length = 15;
	entry.remote_source = 0;
	entry.remote_sequence = 0;

	ringbuffer_push(&log_buffer, &entry);
	commit_log_entry();
}

/*
 * Log an integer to go with the last log entry that has been started.  To make optimal
 * use of space, integers are always packed and logged in lists since the size of each
 * log entry is fixed at 16 bytes.  So, first check if there's an existing open LogEntry
 * and if so add the integer to that one.  Otherwise, create a new one
 *
 */
void log_integer(unsigned int value)
{
	GenericLogEntry *entry = NULL;
	GenericLogEntry new_entry;

	//Check if there is an existing integer list we can append to
	if (!ringbuffer_empty(&log_buffer))
	{
		entry = (GenericLogEntry*)ringbuffer_peeklast(&log_buffer);
		if (entry->header.type == kIntegerListType && entry->header.length < (LOG_DATA_SIZE - 2))
		{
			entry->data[entry->header.length++] = value & 0xFF;
			entry->data[entry->header.length++] = value >> 8;
			return;
		}
	}

	//Otherwise, start a new one
	ensure_log_space();
	new_entry.header.type = kIntegerListType;
	new_entry.header.length = 2;

	new_entry.data[0] = value & 0xFF;
	new_entry.data[1] = value >> 8;
	ringbuffer_push(&log_buffer, &new_entry);
	commit_log_entry();
}

void log_array(LogEntryType type, const unsigned char *data, unsigned int length)
{
	unsigned int i;
	GenericLogEntry entry;

	for (i=0; i<length; i += LOG_DATA_SIZE)
	{
		ensure_log_space();
		if (i == 0)
			entry.header.type = type;
		else
			entry.header.type = kContinuationType;

		entry.header.length = (length - i) < LOG_DATA_SIZE ? (length - i) : LOG_DATA_SIZE;
		memcpy(entry.data, data+i, entry.header.length);
		ringbuffer_push(&log_buffer, &entry);
		commit_log_entry();
	}
}

void log_string(const char *string)
{
	log_array(kStringType, (const unsigned char *)string, strlen(string) + 1);
}

void ensure_log_space()
{
	if (ringbuffer_full(&log_buffer))
		flush_log(NULL); // This will lock things up but we need to make sure we save off the log entries
}

/*
 * Routines for reading the log back after its been written.
 */


#ifndef __NO_FLASH__

bool read_system_log( uint16 index, GenericLogEntry *out)
{
	fq_walker_data walker;

	fqwalker_init(&walker, &log_queue, index);
	
	return fqwalker_next(&walker, out);	
}

void clear_system_log()
{
	fq_clear(&log_queue);
}

uint16 system_log_count()
{
	return fq_count(&log_queue);
}

void flush_log(void* arg)
{
	flush_task_pending = false;
	while ( !ringbuffer_empty( &log_buffer ) )
	{
		fq_push(&log_queue, ringbuffer_peek( &log_buffer ));
		ringbuffer_pop(&log_buffer, NULL);
	}
}

void commit_log_entry()
{
	if (lazy_system_logging && !flush_task_pending)
	{
		flush_task_pending = true;
		taskloop_add(flush_log, NULL);
	}
	else if (!lazy_system_logging)
	{
		flush_log(NULL);
	}
}

#else

bool read_system_log( uint16 index, GenericLogEntry *out)
{
	return false;
}
void clear_system_log()
{
	ringbuffer_reset(&log_buffer);
}

uint16 system_log_count()
{
	return 0;
}

void flush_log(void* arg)
{

}
void commit_log_entry()
{

}

#endif