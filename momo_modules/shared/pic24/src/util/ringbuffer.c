#include "ringbuffer.h"
#include "common_types.h"
#include <string.h>
#include "pic24.h"

void ringbuffer_create(ringbuffer *out, void *data, unsigned int size, unsigned int length)
{
    out->data = (unsigned char*)data;
    out->elem_size = size;
    out->mask = length-1;

    ringbuffer_reset(out);
}

bool ringbuffer_empty(ringbuffer *buf)
{
    return buf->count == 0;
}

/*
 * The ringbuffer is full if the indices have not wrapped the same number of times
 * but still point to the same index when the high bit is masked out.
 */
bool ringbuffer_full(ringbuffer *buf)
{
    return buf->count == (buf->mask + 1);
}

/*
 * it is up to the user to ensure that the ringbuffer has items in it that can be read
 */
void ringbuffer_pop(ringbuffer *buf, void *out)
{
    uninterruptible_start();
    if (out)
        memcpy(out, ringbuffer_peek(buf), buf->elem_size);

    buf->start = (buf->start + 1) & buf->mask;
    --buf->count;
    uninterruptible_end();
}

void* ringbuffer_peek( ringbuffer *buf )
{
    unsigned int offset = (buf->start) & buf->mask;
    return buf->data + (offset*buf->elem_size);
}

void* ringbuffer_peekindex(ringbuffer *buf, int index)
{
    unsigned int offset = (buf->start + index) & buf->mask;
    return buf->data + (offset*buf->elem_size);
}

void* ringbuffer_peeklast( ringbuffer *buf )
{
    unsigned int offset = (buf->start + buf->count - 1) & buf->mask;
    return buf->data + (offset*buf->elem_size);
}

void ringbuffer_push(ringbuffer *buf, void *in)
{
    unsigned int offset;
    uninterruptible_start();
    
    offset = (buf->start + buf->count) & buf->mask;
    memcpy(buf->data + offset*buf->elem_size, in, buf->elem_size);
    
    if (ringbuffer_full(buf))
        buf->start = (buf->start + 1) & buf->mask;
    else
        ++buf->count;

    uninterruptible_end();
}

void ringbuffer_reset(ringbuffer* buf) 
{
    buf->start = 0; //start points to the oldest valid data item
    buf->count = 0;
}