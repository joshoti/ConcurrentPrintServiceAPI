#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/** 
 * @file linked_list.h
 * @brief Header file for linked_list.c, containing function declarations
 *        and the LinkedList structure definition.
 *
 * @note This is a doubly linked list implementation.
 *
 * @note The list does not manage the memory of the objects it contains;
 *       it is the caller's responsibility to free the objects if needed.
 */

// --- Data Structures ---
typedef struct ListNode {
    void* data;
    struct ListNode* next;
    struct ListNode* prev;
} ListNode;

typedef struct {
    int members_count;
    ListNode head;
    ListNode tail;
} LinkedList;

// --- Function Declarations ---
/**
 * @brief Get the number of elements in the list.
 * @param list Pointer to the LinkedList.
 * @return The number of elements in the list.
 */
int  list_length(LinkedList* list);

/**
 * @brief Check if the list is empty.
 * @param list Pointer to the LinkedList.
 * @return 1 if the list is empty, 0 otherwise.
 */
int  list_is_empty(LinkedList* list);

/**
 * @brief Append an object to the end of the list.
 * @param list Pointer to the LinkedList.
 * @param data Pointer to the object to append.
 * @return 1 on success, 0 on failure (e.g., memory allocation failure).
 */
int  list_append(LinkedList* list, void* data);

/**
 * @brief Append an object to the beginning of the list.
 * @param list Pointer to the LinkedList.
 * @param data Pointer to the object to append.
 * @return 1 on success, 0 on failure (e.g., memory allocation failure).
 */
int  list_append_left(LinkedList* list, void* data);

/**
 * @brief Remove and return the last object from the list.
 * @param list Pointer to the LinkedList.
 * @return Pointer to the removed object, or NULL if the list is empty.
 */
ListNode* list_pop(LinkedList* list);

/**
 * @brief Remove and return the first object from the list.
 * @param list Pointer to the LinkedList.
 * @return Pointer to the removed object, or NULL if the list is empty.
 */
ListNode* list_pop_left(LinkedList* list);

/**
 * @brief Remove a specific node from the list.
 * @param list Pointer to the LinkedList.
 * @param node Pointer to the ListNode to remove.
 */
void list_remove(LinkedList* list, ListNode* node);

/**
 * @brief Clear all elements from the list.
 * @param list Pointer to the LinkedList.
 */
void list_clear(LinkedList* list);

/**
 * @brief Get the first node in the list.
 * @param list Pointer to the LinkedList.
 * @return Pointer to the first ListNode, or NULL if the list is empty.
 */
ListNode* list_first(LinkedList* list);

/**
 * @brief Get the last node in the list.
 * @param list Pointer to the LinkedList.
 * @return Pointer to the last ListNode, or NULL if the list is empty.
 */
ListNode* list_last(LinkedList* list);

/**
 * @brief Find a node in the list containing the specified data.
 * @param list Pointer to the LinkedList.
 * @param data Pointer to the data to search for.
 * @return Pointer to the found ListNode, or NULL if not found.
 */
ListNode* list_find(LinkedList* list, void* data);

/**
 * @brief Initialize a LinkedList structure.
 * @param list Pointer to the LinkedList to initialize.
 * @return 1 on success, 0 on failure (e.g., memory allocation failure).
 */
int list_init(LinkedList* list);

#endif // LINKED_LIST_H