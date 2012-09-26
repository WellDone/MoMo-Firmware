#ifndef __ringbuffer_h
#define __ringbuffer_h

//Length of data must be a power of 2
typedef struct
{
    unsigned char   *data;
    unsigned int    elem_size;
    unsigned int    mask; //length of buffer - 1
    
    unsigned int    head;
    unsigned int    tail;
} ringbuffer;

void ringbuffer_create(ringbuffer *out, void *data, unsigned int size, unsigned int length);
void ringbuffer_push(ringbuffer *buf, void *data);
void ringbuffer_pop(ringbuffer *buf, void *out);
unsigned int ringbuffer_count(ringbuffer *buf);
unsigned int ringbuffer_empty(ringbuffer *buf);
unsigned int ringbuffer_full(ringbuffer *buf);

#endif
