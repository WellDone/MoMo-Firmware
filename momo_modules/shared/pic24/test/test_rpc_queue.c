#include "unity.h"
#include "rpc_queue.h"
#include "task_manager.h"
#include "ringbuffer.h"
#include <string.h>

void asm_sleep()
{
	// do nothing
}

static int task_count = 0;
void setUp(void) {
	rpc_queue_init();
	taskloop_init();
}
void tearDown(void) {

}

void noncritical_task_0(void) {
	TEST_ASSERT_EQUAL_INT( 0, task_count );
	TEST_ASSERT_TRUE( !taskloop_locked() );
	++task_count;
}
void critical_task_1(void) {
	TEST_ASSERT_EQUAL_INT( 1, task_count );
	TEST_ASSERT_TRUE( taskloop_locked() );
	++task_count;	
}
void critical_task_2(void) {
	TEST_ASSERT_EQUAL_INT( 2, task_count );
	TEST_ASSERT_TRUE( taskloop_locked() );
	++task_count;	
}
void noncritical_task_3(void) {
	TEST_ASSERT_EQUAL_INT( 3, task_count );
	TEST_ASSERT_TRUE( !taskloop_locked() );
	++task_count;
}
void queue_rpc(void) {
	TEST_ASSERT_FALSE( rpc_queue_full() );
	rpc_queue( NULL, 0, 0, 0, 0 );
}
void dequeue_rpc(void) {
	TEST_ASSERT_FALSE( rpc_queue_empty() );
	rpc_dequeue(NULL);
}

// Like taskloop_loop, but doesn't loop forever
void taskloop_pseudo_loop(void) {
  while( taskloop_process_one() )
    ;
}
void test_rpc_queue_taskloop_locking(void) {
	task_count = 0;
	taskloop_add( noncritical_task_0 );
	int i = 0;
	while ( i++ < RPC_QUEUE_SIZE ) {
		taskloop_add( queue_rpc );
	}
	taskloop_add_critical( critical_task_1 );
	taskloop_add( noncritical_task_3 ); // won't actually run.
	taskloop_add_critical( critical_task_2 );
	taskloop_add_critical( dequeue_rpc );
	//Note that another task inserted here would run before noncritical_task_3 
	//  because ordering is not preserved once the taskloop has been locked.
	//Now noncritical_task_3 should actually run because the taskloop is unlocked

	while (!rpc_queue_empty())
		rpc_dequeue(NULL); // Clear out anything left over
	taskloop_pseudo_loop();
	TEST_ASSERT_EQUAL_INT( 4, task_count );
}

static rpc_info infos[] = {
	{(void*)200,3,17,42,99},
	{(void*)255,6,34,84,198},
	{(void*)0,1,2,3,4}
};
void test_rpc_queue(void) {
	int i;
	int count = sizeof(infos)/sizeof(rpc_info);
	while (!rpc_queue_empty())
		rpc_dequeue(NULL); // Clear out anything left over
	for ( i=0; i<count; ++i ) {
		rpc_queue( infos[i].callback, infos[i].address, infos[i].feature, infos[i].cmd, infos[i].param_spec );
	}

	rpc_info dequeued_info;
	for ( i=0; i<count; ++i ) {
		TEST_ASSERT_FALSE( rpc_queue_empty() );
		rpc_dequeue( &dequeued_info );
		TEST_ASSERT_EQUAL_INT( 0, memcmp( &dequeued_info, &infos[i], sizeof(rpc_info) ) );
	}
	TEST_ASSERT_TRUE( rpc_queue_empty() );
}