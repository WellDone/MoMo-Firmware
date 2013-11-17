#include "rpc_queue.h"
#include "task_manager.h"

static rpc_info   the_rpc_queue_data[RPC_QUEUE_SIZE];
static ringbuffer the_rpc_queue;

void rpc_queue_init()
{
	ringbuffer_create( &the_rpc_queue, the_rpc_queue_data, sizeof(rpc_info), RPC_QUEUE_SIZE );
}
void rpc_queue( mib_rpc_function callback, unsigned char address, unsigned char feature, unsigned char cmd, uint8 spec )
{
	rpc_info info;
	info.callback = callback;
	info.address = address;
	info.feature = feature;
	info.cmd = cmd;
	info.param_spec = spec;
	ringbuffer_push( &the_rpc_queue, &info );

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