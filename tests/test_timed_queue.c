#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "timed_queue.h"
#include "linked_list.h"
#include "test_utils.h"

void print_timestamp(const char* label, unsigned long timestamp_us) {
    unsigned long ms = timestamp_us / 1000;
    int us = timestamp_us % 1000;
    printf("%s: %lu.%03d ms\n", label, ms, us);
}

int test_timed_queue_init(timed_queue_t* tq) {
    int failed = 0;
    if (timed_queue_init(tq) == TRUE) {
        printf("Passed timed queue init test.\n");
        print_timestamp("Timestamp after init", tq->last_interaction_time_us);
    } else {
        printf("Failed timed queue init test.\n");
        failed = 1;
    }
    return failed;
}

int test_timed_queue_enqueue(timed_queue_t* tq, void* obj1, void* obj2, void* obj3, void* obj4) {
    printf("\n--- Testing Enqueue Operations ---\n");
    int failed = 0;
    
    unsigned long time_before = tq->last_interaction_time_us;
    print_timestamp("Timestamp before enqueue", time_before);
    
    // Small delay to ensure timestamp changes
    usleep(1000); // 1ms delay
    
    int result1 = timed_queue_enqueue(tq, obj1);
    printf("Enqueued item 1: %d\n", *(int*)obj1);
    print_timestamp("Timestamp after enqueue 1", tq->last_interaction_time_us);
    
    usleep(500); // 0.5ms delay
    
    int result2 = timed_queue_enqueue(tq, obj2);
    printf("Enqueued item 2: %d\n", *(int*)obj2);
    print_timestamp("Timestamp after enqueue 2", tq->last_interaction_time_us);
    
    usleep(500);
    
    int result3 = timed_queue_enqueue(tq, obj3);
    printf("Enqueued item 3: %d\n", *(int*)obj3);
    print_timestamp("Timestamp after enqueue 3", tq->last_interaction_time_us);
    
    usleep(500);
    
    int result4 = timed_queue_enqueue(tq, obj4);
    printf("Enqueued item 4: %d\n", *(int*)obj4);
    print_timestamp("Timestamp after enqueue 4", tq->last_interaction_time_us);
    
    unsigned long time_after = tq->last_interaction_time_us;
    
    if (result1 && result2 && result3 && result4) {
        printf("\nPassed timed queue enqueue test.\n");
        printf("Queue length: %d\n", timed_queue_length(tq));
        
        if (time_after > time_before) {
            printf("Passed timestamp update test (timestamp increased).\n");
        } else {
            printf("Failed timestamp update test (timestamp did not increase).\n");
            failed = 1;
        }
    } else {
        printf("\nFailed timed queue enqueue test.\n");
        failed = 1;
    }
    return failed;
}

int test_timed_queue_dequeue(timed_queue_t* tq, int expected_value) {
    printf("\n--- Testing Dequeue Operation ---\n");
    int failed = 0;
    
    unsigned long time_before = tq->last_interaction_time_us;
    print_timestamp("Timestamp before dequeue", time_before);
    
    // Small delay to ensure timestamp changes
    usleep(1000); // 1ms delay
    
    list_node_t* node = timed_queue_dequeue_front(tq);
    
    unsigned long time_after = tq->last_interaction_time_us;
    print_timestamp("Timestamp after dequeue", time_after);
    
    if (node != NULL) {
        int value = *(int*)node->data;
        printf("Dequeued value: %d (expected: %d)\n", value, expected_value);
        printf("Queue length after dequeue: %d\n", timed_queue_length(tq));
        
        if (value == expected_value) {
            printf("Passed dequeue value test.\n");
        } else {
            printf("Failed dequeue value test.\n");
            failed = 1;
        }
        
        if (time_after > time_before) {
            printf("Passed timestamp update test (timestamp increased after dequeue).\n");
        } else {
            printf("Failed timestamp update test (timestamp did not increase after dequeue).\n");
            failed = 1;
        }
        
        free(node);
    } else {
        printf("Failed dequeue test (returned NULL).\n");
        failed = 1;
    }
    return failed;
}

int test_read_only_operations(timed_queue_t* tq) {
    printf("\n--- Testing Read-Only Operations (Should NOT Update Timestamp) ---\n");
    int failed = 0;
    
    unsigned long time_before = tq->last_interaction_time_us;
    print_timestamp("Timestamp before read operations", time_before);
    
    // Test timed_queue_first (read-only)
    list_node_t* first = timed_queue_first(tq);
    if (first != NULL) {
        printf("First element: %d\n", *(int*)first->data);
    }
    
    // Test timed_queue_last (read-only)
    list_node_t* last = timed_queue_last(tq);
    if (last != NULL) {
        printf("Last element: %d\n", *(int*)last->data);
    }
    
    // Test timed_queue_length (read-only)
    int length = timed_queue_length(tq);
    printf("Queue length: %d\n", length);
    
    // Test timed_queue_is_empty (read-only)
    int is_empty = timed_queue_is_empty(tq);
    printf("Queue is empty: %d\n", is_empty);
    
    unsigned long time_after = tq->last_interaction_time_us;
    print_timestamp("Timestamp after read operations", time_after);
    
    if (time_after == time_before) {
        printf("Passed read-only test (timestamp unchanged).\n");
    } else {
        printf("Failed read-only test (timestamp should not change for read operations).\n");
        failed = 1;
    }
    return failed;
}

void print_all_queue_elements(timed_queue_t* tq) {
    printf("\n--- Current Queue Contents ---\n");
    list_node_t* curr = timed_queue_first(tq);
    int position = 0;
    
    if (curr == NULL) {
        printf("Queue is empty.\n");
        return;
    }
    
    while (curr && curr != &tq->list.tail) {
        printf("Position %d: %d\n", position++, *(int*)curr->data);
        curr = curr->next;
    }
    printf("Total elements: %d\n", position);
}

int main() {
    char test_name[] = "TIMED QUEUE";
    print_test_start(test_name);

    int failed_test_count = 0;
    
    // Initialize timed queue
    timed_queue_t tq;
    test_timed_queue_init(&tq);
    
    printf("\n=================================================\n");
    
    // Create test data
    int a = 10, b = 20, c = 30, d = 40;
    
    // Test enqueuing 4 items
    test_timed_queue_enqueue(&tq, &a, &b, &c, &d);
    
    // Print all elements
    print_all_queue_elements(&tq);
    
    printf("\n=================================================\n");
    
    // Test read-only operations (should not change timestamp)
    test_read_only_operations(&tq);
    
    printf("\n=================================================\n");
    
    // Test dequeue (should remove first item - value 10)
    test_timed_queue_dequeue(&tq, 10);
    
    // Print remaining elements
    print_all_queue_elements(&tq);
    
    printf("\n=================================================\n");
    
    // Test another dequeue
    printf("\nDequeuing second element...\n");
    test_timed_queue_dequeue(&tq, 20);
    
    // Print remaining elements
    print_all_queue_elements(&tq);
    
    printf("\n=================================================\n");
    
    // Test clear operation
    printf("\n--- Testing Clear Operation ---\n");
    unsigned long time_before_clear = tq.last_interaction_time_us;
    print_timestamp("Timestamp before clear", time_before_clear);
    
    usleep(1000);
    timed_queue_clear(&tq);
    
    unsigned long time_after_clear = tq.last_interaction_time_us;
    print_timestamp("Timestamp after clear", time_after_clear);
    
    printf("Queue is empty after clear: %d\n", timed_queue_is_empty(&tq));
    
    if (time_after_clear > time_before_clear) {
        printf("Passed clear timestamp update test.\n");
    } else {
        printf("Failed clear timestamp update test.\n");
        failed_test_count++;
    }
    
    print_test_end(test_name, failed_test_count);
    return 0;
}
