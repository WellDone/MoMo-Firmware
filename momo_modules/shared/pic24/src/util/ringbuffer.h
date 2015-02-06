#ifndef __ringbuffer_h
#define __ringbuffer_h

#include "platform.h"

/*
 * A simple circular buffer with variable sized elements.  The buffer size
 * must be a power of two for speed in masking offsets.
 */
//Implementation inspired by: http://en.wikipedia.org/wiki/Circular_buffer

//N.B. Length must be a power of two!
typedef struct
{
    unsigned char   *data;
    unsigned int    elem_size;
    unsigned int    mask;

    unsigned int    start; //the first valid item
    unsigned int    count; //the number of valid items
} ringbuffer;

void ringbuffer_create(ringbuffer *out, void *data, unsigned int size, unsigned int length);
void ringbuffer_push(ringbuffer *buf, void *data);
void ringbuffer_pop(ringbuffer *buf, void *out);
void* ringbuffer_peek(ringbuffer *buf);
void* ringbuffer_peekindex(ringbuffer *buf, int index);
void* ringbuffer_peeklast( ringbuffer *buf );
bool ringbuffer_empty(ringbuffer *buf);
bool ringbuffer_full(ringbuffer *buf);
void ringbuffer_reset(ringbuffer *buf);

#endif