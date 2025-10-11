#include <stdlib.h>
#include "timed_queue.h"
#include "linked_list.h"
#include "timeutils.h"
#include "common.h"

int timed_queue_init(TimedQueue* tq) {
    if (tq == NULL) {
        return FALSE;
    }
    
    int result = list_init(&tq->list);
    if (result) {
        tq->last_interaction_time_us = get_time_in_us();
    }
    return result;
}

int timed_queue_length(TimedQueue* tq) {
    if (tq == NULL) {
        return 0;
    }
    return list_length(&tq->list);
}

int timed_queue_is_empty(TimedQueue* tq) {
    if (tq == NULL) {
        return TRUE;
    }
    return list_is_empty(&tq->list);
}

int timed_queue_enqueue(TimedQueue* tq, void* data) {
    if (tq == NULL) {
        return FALSE;
    }
    
    int result = list_append(&tq->list, data);
    if (result) {
        tq->last_interaction_time_us = get_time_in_us();
    }
    return result;
}

int timed_queue_enqueue_front(TimedQueue* tq, void* data) {
    if (tq == NULL) {
        return FALSE;
    }
    
    int result = list_append_left(&tq->list, data);
    if (result) {
        tq->last_interaction_time_us = get_time_in_us();
    }
    return result;
}

ListNode* timed_queue_dequeue(TimedQueue* tq) {
    if (tq == NULL) {
        return NULL;
    }
    
    ListNode* node = list_pop(&tq->list);
    if (node != NULL) {
        tq->last_interaction_time_us = get_time_in_us();
    }
    return node;
}

ListNode* timed_queue_dequeue_front(TimedQueue* tq) {
    if (tq == NULL) {
        return NULL;
    }
    
    ListNode* node = list_pop_left(&tq->list);
    if (node != NULL) {
        tq->last_interaction_time_us = get_time_in_us();
    }
    return node;
}

void timed_queue_remove(TimedQueue* tq, ListNode* node) {
    if (tq == NULL || node == NULL) {
        return;
    }
    
    list_remove(&tq->list, node);
    tq->last_interaction_time_us = get_time_in_us();
}

void timed_queue_clear(TimedQueue* tq) {
    if (tq == NULL) {
        return;
    }
    
    list_clear(&tq->list);
    tq->last_interaction_time_us = get_time_in_us();
}

ListNode* timed_queue_first(TimedQueue* tq) {
    if (tq == NULL) {
        return NULL;
    }
    // Read-only operation - does NOT update timestamp
    return list_first(&tq->list);
}

ListNode* timed_queue_last(TimedQueue* tq) {
    if (tq == NULL) {
        return NULL;
    }
    // Read-only operation - does NOT update timestamp
    return list_last(&tq->list);
}

ListNode* timed_queue_find(TimedQueue* tq, void* data) {
    if (tq == NULL) {
        return NULL;
    }
    // Read-only operation - does NOT update timestamp
    return list_find(&tq->list, data);
}
