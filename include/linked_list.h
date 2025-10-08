#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct ListNode {
    void* data;
    struct ListNode* next;
    struct ListNode* prev;
} ListNode;

typedef struct {
    int num_members;
    ListNode head;
    ListNode tail;
} LinkedList;

int  list_length(LinkedList*);
int  list_is_empty(LinkedList*);

int  list_append(LinkedList*, void*);
int  list_append_left(LinkedList*, void*);
ListNode* list_pop(LinkedList*);
ListNode* list_pop_left(LinkedList*);
void list_remove(LinkedList*, ListNode*);
void list_clear(LinkedList*);

ListNode* list_first(LinkedList*);
ListNode* list_last(LinkedList*);
ListNode* list_next(LinkedList*, ListNode*);
ListNode* list_prev(LinkedList*, ListNode*);

ListNode* list_find(LinkedList*, void*);

int list_init(LinkedList*);

#endif // LINKED_LIST_H