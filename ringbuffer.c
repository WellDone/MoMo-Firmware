#include "ringbuffer.h"
#include <stdlib.h>

void ringbuffer_create(ringbuffer *out, void *data, unsigned int size, unsigned int length)
{
    out->data = (unsigned char*)data;
    out->elem_size = size;
    out->mask = length-1;
    out->head = 0; //head points to the next location to be written
    out->tail = 0; //tail points to next location to be read
}

unsigned int ringbuffer_count(ringbuffer *buf)
{
    (buf->head - buf->tail) & buf->mask; //If tail is > head, this still works due to wrapping of unsigned arithmetic
}

unsigned int ringbuffer_empty(ringbuffer *buf)
{
    return (buf->head == buf->tail);
}

/*
 * We onlt store length - 1 values in ringbuffer to make the math easier
 * Probably a FIXME...
 */
unsigned int ringbuffer_full(ringbuffer *buf)
{
    return (ringbuffer_count(buf) == buf->mask);
}

unsigned int ringbuffer_space(ringbuffer *buf)
{
    return buf->mask - ringbuffer_count(buf);
}

/*
 * it is up to the user to ensure that the ringbuffer has items in it that can be read
 */
void ringbuffer_pop(ringbuffer *buf, void *out)
{
    memcpy(out, buf->data + buf->elem_size*(buf->tail++ & buf->mask), buf->elem_size);
}

void ringbuffer_push(ringbuffer *buf, void *in)
{
    memcpy(buf->data + buf->elem_size*(buf->head++ & buf->mask), buf->elem_size);
}