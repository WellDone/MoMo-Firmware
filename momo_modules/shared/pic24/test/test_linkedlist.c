//Name: test_linkedlist
//Type: module

#include "unity.h"
#include "linkedlist.h"
#include <xc.h>
#include <stdio.h>

void setUp(void) 
{

}
void tearDown(void) 
{

}

void test_linkedlist()
{
	linked_t item1, item2, item3;
	linked_t *head = NULL;

	TEST_ASSERT_NULL(head);

	list_push_real(&head, &item1);

	TEST_ASSERT_NOT_NULL(head);
	TEST_ASSERT_EQUAL_PTR(head, &item1);

	list_push_real(&head, &item2);
	list_push_real(&head, &item3);

	TEST_ASSERT_EQUAL_PTR(list_pop_real(&head), &item1);
	TEST_ASSERT_EQUAL_PTR(list_pop(&head, linked_t), &item2);
	TEST_ASSERT_EQUAL_PTR(list_pop(&head, linked_t), &item3);
	TEST_ASSERT_NULL(list_pop(&head, linked_t));
	TEST_ASSERT_NULL(head);
}