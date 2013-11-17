#ifndef __rpc_queue_h
#define __rpc_queue_h

#include "protocol.h"
#include "ringbuffer.h"

#define RPC_QUEUE_SIZE 4 // Must be a power of 2

typedef struct {
	mib_rpc_function callback;
	unsigned char address;
	unsigned char feature;
	unsigned char cmd;
	uint8 param_spec;
} rpc_info;

void rpc_queue_init();
void rpc_queue( mib_rpc_function callback, unsigned char address, unsigned char feature, unsigned char cmd, uint8 spec );
inline void rpc_dequeue(rpc_info* out);
inline const rpc_info* rpc_queue_peek();
inline bool rpc_queue_full();
inline bool rpc_queue_empty();

#endif