#include <stdlib.h>
#include "common.h"
#include "linked_list.h"


int list_length(LinkedList* list) {
    return list->members_count;
}
int list_is_empty(LinkedList* list) {
    return list->members_count == 0;
}

int list_append(LinkedList* list, void* obj) {
    ListNode* newNode = (ListNode*) malloc(sizeof(ListNode));
    if (newNode == NULL) {
        return FALSE; // Memory allocation failure
    }
    newNode->data = obj;
    newNode->next = &list->tail;
    newNode->prev = list->tail.prev;

    list->tail.prev->next = newNode;
    list->tail.prev = newNode;
    list->members_count++;

    return TRUE;
}

int list_append_left(LinkedList* list, void* obj) {
    ListNode* newNode = (ListNode*) malloc(sizeof(ListNode));
    if (newNode == NULL) {
        return FALSE; // Memory allocation failure
    }
    newNode->data = obj;
    newNode->next = list->head.next;
    newNode->prev = &list->head;

    list->head.next->prev = newNode;
    list->head.next = newNode;
    list->members_count++;

    return TRUE;
}
ListNode* list_pop(LinkedList* list) {
    if (list_is_empty(list)) {
        return NULL;
    }
    ListNode* last = list->tail.prev;
    last->prev->next = &list->tail;
    list->tail.prev = last->prev;
    list->members_count--;
    return last;
}

ListNode* list_pop_left(LinkedList* list) {
    if (list_is_empty(list)) {
        return NULL;
    }
    ListNode* first = list->head.next;
    first->next->prev = &list->head;
    list->head.next = first->next;
    list->members_count--;
    return first;
}

void list_remove(LinkedList* list, ListNode* node) {
    if (node == NULL || list_is_empty(list)) {
        return;
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    list->members_count--;
    free(node);
}

void list_clear(LinkedList* list) {
    while (!list_is_empty(list)) {
        ListNode* node = list_pop_left(list);
        free(node);
    }
}

ListNode* list_first(LinkedList* list) {
    if (list_is_empty(list)) {
        return NULL;
    }
    return list->head.next;
}

ListNode* list_last(LinkedList* list) {
    if (list_is_empty(list)) {
        return NULL;
    }
    return list->tail.prev;
}

ListNode* list_find(LinkedList* list, void* data) {
    ListNode* curr = NULL;
    for (curr = list_first(list); curr != NULL; curr = curr->next) {
        if (curr->data == data) {
            return curr;
        }
    }
    return NULL;
}

int list_init(LinkedList* list) {
    if (list == NULL) {
        return FALSE; // Invalid list pointer
    }
    list->members_count = 0;
    list->head.next = &list->tail;
    list->head.prev = NULL;
    list->head.data = NULL;

    list->tail.prev = &list->head;
    list->tail.next = NULL;
    list->tail.data = NULL;

    return TRUE;
}