//Name: test_ringbuffer
//Type: module
//Description: Tests to make sure that the ringbuffer module is working correctly.

#include "unity.h"
#include "ringbuffer.h"

#define BUFSIZE 8

ringbuffer test;
unsigned int data[BUFSIZE];

void setUp(void) 
{
	ringbuffer_create(&test, data, sizeof(unsigned int), BUFSIZE);
}
void tearDown(void) 
{

}

void test_ringbuffer_pushing(void)
{
	unsigned int i;

	TEST_ASSERT_TRUE(ringbuffer_empty(&test));

	for (i=1; i<=BUFSIZE; ++i)
	{
		ringbuffer_push(&test, &i);
		TEST_ASSERT_EQUAL_INT(i, test.count);
	}

	TEST_ASSERT_TRUE(ringbuffer_full(&test));
}

void test_ringbuffer_popping(void)
{
	unsigned int i;

	TEST_ASSERT_TRUE(ringbuffer_empty(&test));

	for (i=1; i<=BUFSIZE; ++i)
		ringbuffer_push(&test, &i);

	for (i=1; i<=BUFSIZE; ++i)
	{
		unsigned int val;

		ringbuffer_pop(&test, &val);
		TEST_ASSERT_EQUAL_INT(i, val);
		TEST_ASSERT_EQUAL_INT(BUFSIZE-i, test.count);
	}

	TEST_ASSERT_TRUE(ringbuffer_empty(&test));
}

void test_ringbuffer_overflow(void)
{
	unsigned int i;
	unsigned int val;

	TEST_ASSERT_TRUE(ringbuffer_empty(&test));

	for (i=1; i<=BUFSIZE; ++i)
		ringbuffer_push(&test, &i);

	TEST_ASSERT_TRUE(ringbuffer_full(&test));
	TEST_ASSERT_EQUAL_INT(BUFSIZE, test.count);

	val = 9;
	ringbuffer_push(&test, &val);
	TEST_ASSERT_TRUE(ringbuffer_full(&test));
	TEST_ASSERT_EQUAL_INT(BUFSIZE, test.count);

	ringbuffer_pop(&test, &val);
	TEST_ASSERT_EQUAL_INT(2, val);
	TEST_ASSERT_EQUAL_INT(BUFSIZE-1, test.count);
}

void test_ringbuffer_peek(void)
{
	unsigned int i;
	unsigned int val = 10;
	unsigned int *first;

	TEST_ASSERT_TRUE(ringbuffer_empty(&test));

	for (i=1; i<=BUFSIZE; ++i)
		ringbuffer_push(&test, &i);

	ringbuffer_push(&test, &val);
	ringbuffer_push(&test, &val);
	first = ringbuffer_peek(&test);
	TEST_ASSERT_EQUAL_INT(3, *first);
}

void test_ringbuffer_peeklast(void)
{
	unsigned int i;
	unsigned int val = 10;
	unsigned int *last;

	TEST_ASSERT_TRUE(ringbuffer_empty(&test));

	for (i=1; i<=BUFSIZE; ++i)
	{
		ringbuffer_push(&test, &i);
		last = ringbuffer_peeklast(&test);
		TEST_ASSERT_EQUAL_INT(i, *last);
	}

	val = 9;
	ringbuffer_push(&test, &val);
	last = ringbuffer_peeklast(&test);
	TEST_ASSERT_EQUAL_INT(val, *last);

	val = 10;
	ringbuffer_push(&test, &val);
	last = ringbuffer_peeklast(&test);
	TEST_ASSERT_EQUAL_INT(val, *last);
}