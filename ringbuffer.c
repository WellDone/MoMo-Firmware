#include "ringbuffer.h"
#include "common.h"
#include <string.h>

void ringbuffer_create(ringbuffer *out, void *data, unsigned int size, unsigned int length)
{
    out->data = (unsigned char*)data;
    out->elem_size = size;
    out->length = length;
    out->start = 0; //start points to the oldest valid data item
    out->end = 0; //end points one past the newest valid data item
}

unsigned int ringbuffer_empty(ringbuffer *buf)
{
    return (buf->start == buf->end);
}

/*
 * The ringbuffer is full if the indices have not wrapped the same number of times
 * but still point to the same index when the high bit is masked out.
 */
unsigned int ringbuffer_full(ringbuffer *buf)
{
    return (buf->end == (buf->start ^ buf->length));
}

/*
 * it is up to the user to ensure that the ringbuffer has items in it that can be read
 */
void ringbuffer_pop(ringbuffer *buf, void *out)
{
    unsigned int mask = buf->length - 1;
    unsigned int offset = (buf->start) & mask;
   
    memcpy(out, buf->data + (offset*buf->elem_size), buf->elem_size);

    ringbuffer_incr(buf, &buf->start);
}

/*
 * If ringbuffer is full, the oldest item is overwritten.
 */
void ringbuffer_push(ringbuffer *buf, void *in)
{
    unsigned int mask = buf->length - 1;
    unsigned int offset = (buf->end) & mask;
    unsigned int i;

    memcpy(buf->data + offset*buf->elem_size, in, buf->elem_size);
    if (ringbuffer_full(buf))
        ringbuffer_incr(buf, &buf->start); //We overflowed so we have to increment start too.
    ringbuffer_incr(buf, &buf->end);
}

//FIXME, should disable interrupts during this routine so that we don't have a race
//with a preempted interrupt on the non-atomic increment operation.
static void ringbuffer_incr(ringbuffer *buf, unsigned int *index)
{
    *index = (*index+1) & (2*buf->length - 1);
}