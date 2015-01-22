#ifndef __linkedlist_h__
#define __linkedlist_h__

/*
 * Functions and Macros for dealing with linked lists
 */

typedef struct linked_tag
{
	struct linked_tag *next;
} linked_t;

//Must be included at the start of any structure that will be used with the linked list functions and macros
#define LINKED_LIST()				linked_t *next

//Convenience Macros
#define list_next(current, type)	(type)current->next
#define list_empty(head)			(head == NULL)
#define list_pop(head, type)		((type*)list_pop_real((linked_t**)(head)))
#define list_push(head, obj)		list_push_real((linked_t**)(head), (linked_t*)(obj))

//Public API Functions
void  list_push_real(linked_t **head, linked_t *obj);
linked_t* list_pop_real(linked_t **head);


#endif