#ifndef __flashlog_h__
#define __flashlog_h__

#include "flashblock.h"

/*
 * flashlog - A flash based timestamped log of arbitrary data
 * 
 * The flashlog stores blobs of data with timestamps in a flash
 * based ringbuffer.  It optionally supports caching in RAM for
 * speed or power consumption optimization.  
 */

typedef struct
{
	uint32_t	readptr;
	uint32_t	writeptr;
} fb_state;

typedef struct
{
	flash_block_info	control_block;
	uint32_t			log_start;
	uint32_t			log_length;
	uint16_t			elem_size;

	fb_state			state;
	unsigned char		*cache;

	union
	{
		struct
		{	
			uint16_t	cache : 1;
			uint16_t	reserved: 15;
		};

		uint16_t flags;
	};			
};

#endif