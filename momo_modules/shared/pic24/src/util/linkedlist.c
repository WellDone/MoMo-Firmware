#include "linkedlist.h"
#include "pic24.h"

void list_push_real(linked_t **head, linked_t *obj)
{
	linked_t *curr;

	if (obj == NULL)
		return;

	uninterruptible_start();

	obj->next = NULL;
	curr = *head;
	

	if (*head == 0)
		*head = obj;
	else
	{
		while(curr->next != NULL)
			curr = curr->next;

		curr->next = obj; 
	}

	uninterruptible_end();
}

linked_t* 	list_pop_real(linked_t **head)
{
	linked_t *curr;

	uninterruptible_start();
	curr = *head;

	if (curr != NULL)
		*head = curr->next;

	uninterruptible_end();

	return curr;
}