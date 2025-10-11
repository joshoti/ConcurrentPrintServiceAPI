#ifndef TIMED_QUEUE_H
#define TIMED_QUEUE_H

#include "linked_list.h"

/**
 * @file timed_queue.h
 * @brief Wrapper around LinkedList that automatically tracks last interaction time.
 *
 * @note This queue automatically updates the last_interaction_time_us field
 *       whenever items are added or removed.
 */

typedef struct TimedQueue {
    LinkedList list;
    unsigned long last_interaction_time_us;
} TimedQueue;

// --- Function Declarations ---

/**
 * @brief Initialize a TimedQueue structure.
 * @param tq Pointer to the TimedQueue to initialize.
 * @return 1 on success, 0 on failure.
 */
int timed_queue_init(TimedQueue* tq);

/**
 * @brief Get the number of elements in the queue.
 * @param tq Pointer to the TimedQueue.
 * @return The number of elements in the queue.
 */
int timed_queue_length(TimedQueue* tq);

/**
 * @brief Check if the queue is empty.
 * @param tq Pointer to the TimedQueue.
 * @return 1 if the queue is empty, 0 otherwise.
 */
int timed_queue_is_empty(TimedQueue* tq);

/**
 * @brief Enqueue (append) an object to the end of the queue.
 * Automatically updates the last_interaction_time_us.
 * @param tq Pointer to the TimedQueue.
 * @param data Pointer to the object to enqueue.
 * @return 1 on success, 0 on failure (e.g., memory allocation failure).
 */
int timed_queue_enqueue(TimedQueue* tq, void* data);

/**
 * @brief Enqueue (append) an object to the front of the queue.
 * Automatically updates the last_interaction_time_us.
 * @param tq Pointer to the TimedQueue.
 * @param data Pointer to the object to enqueue.
 * @return 1 on success, 0 on failure (e.g., memory allocation failure).
 */
int timed_queue_enqueue_front(TimedQueue* tq, void* data);

/**
 * @brief Dequeue (remove) and return the last object from the queue.
 * Automatically updates the last_interaction_time_us.
 * @param tq Pointer to the TimedQueue.
 * @return Pointer to the removed ListNode, or NULL if the queue is empty.
 */
ListNode* timed_queue_dequeue(TimedQueue* tq);

/**
 * @brief Dequeue (remove) and return the first object from the queue.
 * Automatically updates the last_interaction_time_us.
 * @param tq Pointer to the TimedQueue.
 * @return Pointer to the removed ListNode, or NULL if the queue is empty.
 */
ListNode* timed_queue_dequeue_front(TimedQueue* tq);

/**
 * @brief Remove a specific node from the queue.
 * Automatically updates the last_interaction_time_us.
 * @param tq Pointer to the TimedQueue.
 * @param node Pointer to the ListNode to remove.
 */
void timed_queue_remove(TimedQueue* tq, ListNode* node);

/**
 * @brief Clear all elements from the queue.
 * Automatically updates the last_interaction_time_us.
 * @param tq Pointer to the TimedQueue.
 */
void timed_queue_clear(TimedQueue* tq);

/**
 * @brief Get the first node in the queue without removing it.
 * Does NOT update the timestamp (read-only operation).
 * @param tq Pointer to the TimedQueue.
 * @return Pointer to the first ListNode, or NULL if the queue is empty.
 */
ListNode* timed_queue_first(TimedQueue* tq);

/**
 * @brief Get the last node in the queue without removing it.
 * Does NOT update the timestamp (read-only operation).
 * @param tq Pointer to the TimedQueue.
 * @return Pointer to the last ListNode, or NULL if the queue is empty.
 */
ListNode* timed_queue_last(TimedQueue* tq);

/**
 * @brief Find a node in the queue containing the specified data.
 * Does NOT update the timestamp (read-only operation).
 * @param tq Pointer to the TimedQueue.
 * @param data Pointer to the data to search for.
 * @return Pointer to the found ListNode, or NULL if not found.
 */
ListNode* timed_queue_find(TimedQueue* tq, void* data);

#endif // TIMED_QUEUE_H