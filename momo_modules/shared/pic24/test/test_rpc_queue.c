//Name: test_rpc_queue
//Type: module
//Modules: task_manager, ringbuffer, interrupts
//Sources:../../portable

#include "unity.h"
#include "rpc_queue.h"
#include "task_manager.h"
#include "system_log.h"
#include "ringbuffer.h"
#include <string.h>

//Mocked functions
void asm_sleep()
{
	// do nothing
}

void asm_reset()
{

}

void disable_memory()
{

}

void mem_wait_while_writing()
{

}

unsigned int bus_master_idle()
{
	return 1;
}

unsigned int memory_enabled()
{
	return 0;
}

void flush_log(void *arg)
{

}

void write_system_log( LogStream stream, const BYTE* data, uint8 length )
{

}

static int task_count = 0;
void setUp(void) {
	rpc_queue_init();
	taskloop_init();
}
void tearDown(void) {

}

void noncritical_task_0(void *arg) {
	TEST_ASSERT_EQUAL_INT( 0, task_count );
	TEST_ASSERT_TRUE( !taskloop_locked() );
	++task_count;
}
void critical_task_1(void *arg) {
	TEST_ASSERT_EQUAL_INT( 1, task_count );
	TEST_ASSERT_TRUE( taskloop_locked() );
	++task_count;	
}
void critical_task_2(void *arg) {
	TEST_ASSERT_EQUAL_INT( 2, task_count );
	TEST_ASSERT_TRUE( taskloop_locked() );
	++task_count;	
}
void noncritical_task_3(void *arg) {
	TEST_ASSERT_EQUAL_INT( 3, task_count );
	TEST_ASSERT_TRUE( !taskloop_locked() );
	++task_count;
}
void queue_rpc(void *arg) {
	MIBUnified data;
	TEST_ASSERT_FALSE( rpc_queue_full() );
	rpc_queue( NULL, &data);
}
void dequeue_rpc(void *arg) {
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
	taskloop_add( noncritical_task_0, NULL );
	int i = 0;
	while ( i++ < RPC_QUEUE_SIZE ) {
		taskloop_add( queue_rpc, NULL );
	}
	taskloop_add_critical( critical_task_1, NULL );
	taskloop_add( noncritical_task_3, NULL ); // won't actually run.
	taskloop_add_critical( critical_task_2, NULL );
	taskloop_add_critical( dequeue_rpc, NULL );
	//Note that another task inserted here would run before noncritical_task_3 
	//  because ordering is not preserved once the taskloop has been locked.
	//Now noncritical_task_3 should actually run because the taskloop is unlocked

	while (!rpc_queue_empty())
		rpc_dequeue(NULL); // Clear out anything left over
	taskloop_pseudo_loop();
	TEST_ASSERT_EQUAL_INT( 4, task_count );
}