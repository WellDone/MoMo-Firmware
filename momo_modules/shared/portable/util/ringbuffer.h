#ifndef __ringbuffer_h
#define __ringbuffer_h

#include "platform.h"

#ifndef _PIC12LEAN
//The number of elements must be less that than 16K since we use the high bit
//of the indices to tell if the buffer is empty or full.
//Implementation inspired by: http://en.wikipedia.org/wiki/Circular_buffer

//N.B. Length must be a power of two!
typedef struct
{
    unsigned char   *data;
    unsigned int    elem_size;
    unsigned int    length;

    unsigned int    start; //the first valid item
    unsigned int    end; //one past the last valid item
} ringbuffer;

void ringbuffer_create(ringbuffer *out, void *data, unsigned int size, unsigned int length);
void ringbuffer_push(ringbuffer *buf, void *data);
void ringbuffer_pop(ringbuffer *buf, void *out);
unsigned int ringbuffer_empty(ringbuffer *buf);
unsigned int ringbuffer_full(ringbuffer *buf);
void ringbuffer_reset(ringbuffer *buf);

#endif

#endif