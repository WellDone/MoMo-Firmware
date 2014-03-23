#include "rpc_queue.h"
#include "task_manager.h"
#include <string.h>

static rpc_info   the_rpc_queue_data[RPC_QUEUE_SIZE];
static ringbuffer the_rpc_queue;

void rpc_queue_init()
{
	ringbuffer_create( &the_rpc_queue, the_rpc_queue_data, sizeof(rpc_info), RPC_QUEUE_SIZE );
}

void rpc_queue(mib_rpc_function callback, const MIBUnified *data)
{
	rpc_info info;
	info.callback = callback;
	memcpy(&info.data, data, sizeof(MIBUnified));

	ringbuffer_push(&the_rpc_queue, &info);

	if ( rpc_queue_full() )
		taskloop_lock();
}

inline void rpc_dequeue(rpc_info* out)
{
	ringbuffer_pop( &the_rpc_queue, out );
	taskloop_unlock();
}
inline const rpc_info* rpc_queue_peek()
{
	return ringbuffer_peek( &the_rpc_queue);
}

inline bool rpc_queue_full()
{
	return ringbuffer_full( &the_rpc_queue );
}
inline bool rpc_queue_empty()
{
	return ringbuffer_empty( &the_rpc_queue );
}