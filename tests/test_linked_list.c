#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "linked_list.h"
#include "test_utils.h"

static void trim(char *string) {
    // Remove leading spaces
    int i = 0, j = 0;
    while (isspace(string[i])) i++; // Skip leading spaces
    while ((string[j++] = string[i++])); // Shift the characters of string to remove leading spaces

    // Remove trailing spaces
    int len = strlen(string);
    while (len > 0 && isspace((unsigned char)string[len - 1])) {
        string[--len] = '\0';
    }
}

int test_list_length(linked_list_t* list) {
    return list_length(list);
}

int test_list_is_empty(linked_list_t* list) {
    return list_is_empty(list);
}

void test_list_append(linked_list_t* list, void* obj1, void* obj2, void* obj3) {
    int result1 = list_append(list, obj1);
    int result2 = list_append(list, obj2);
    int result3 = list_append(list, obj3);
    if (result1 && result2 && result3) {
        printf("Passed list append test.\n");
    } else {
        printf("Failed list append test.\n");
    }
}

int test_list_append_left(linked_list_t* list, void* obj) {
    return list_append_left(list, obj);
}

list_node_t* test_list_pop(linked_list_t* list) {
    return list_pop(list);
}

list_node_t* test_list_pop_left(linked_list_t* list) {
    return list_pop_left(list);
}

void test_list_remove(linked_list_t* list, list_node_t* node) {
    list_remove(list, node);
}

void test_list_clear(linked_list_t* list) {
    list_clear(list);
}

list_node_t* test_list_first(linked_list_t* list) {
    return list_first(list);
}

list_node_t* test_list_last(linked_list_t* list) {
    return list_last(list);
}

list_node_t* test_list_find(linked_list_t* list, void* data) {
    return list_find(list, data);
}

void test_bad_list_init(linked_list_t* list) {
    int result = list_init(list) == FALSE;
    if (result) {
        printf("Passed bad list init test.\n");
    } else {
        printf("Failed bad list init test.\n");
    }
}

void test_list_init(linked_list_t* list) {
    if (list_init(list) == TRUE) {
        printf("Passed list init test.\n");
    } else {
        printf("Failed list init test.\n");
    }
}

int print_all_elements_and_compare(linked_list_t* list, char* expected) {
    list_node_t* curr = list_first(list);
    char actual[256] = "";
    int failed = 0;
    while (curr && curr != &list->tail) {
        sprintf(actual, "%s%d ", actual, *(int*)curr->data);
        curr = curr->next;
    }
    // Trim leading/trailing spaces for accurate comparison
    trim(actual);

    printf("\nList elements: %s\n", actual);
    printf("Expecting: %s\n", expected);
    if (strcmp(actual, expected) != 0) {
        printf("Failed: List elements do not match expected values.\n");
        failed = 1;
    } else {
       printf("Passed: List elements match expected values.\n");
    }
    printf("\n");
    return failed;
}

int main() {
    char test_name[] = "LINKED LIST";
    print_test_start(test_name);

    int failed_test_count = 0;
    test_bad_list_init(NULL);
    
    linked_list_t list;
    test_list_init(&list);
    
    // Test appending elements
    int a = 1, b = 2, c = 3;
    test_list_append(&list, &a, &b, &c);

    failed_test_count += print_all_elements_and_compare(&list, "1 2 3");
    
    // Test list length
    if (test_list_length(&list) == 3) {
        printf("Passed list append and length test.\n");
    } else {
        printf("Failed list append and length test.\n");
        failed_test_count++;
    }
    
    // Test first and last elements
    list_node_t* first = test_list_first(&list);
    list_node_t* last = test_list_last(&list);
    if (first && last && *(int*)first->data == 1 && *(int*)last->data == 3) {
        printf("Passed first and last element test.\n");
    } else {
        printf("Failed first and last element test.\n");
        failed_test_count++;
    }
    
    // Test if list is empty
    printf("List is empty, should be 0: %d\n", test_list_is_empty(&list));
    
    // Test popping elements
    list_node_t* popped = test_list_pop(&list);
    printf("Popped element, should be 3: %d\n", *(int*)popped->data);
    free(popped);
    
    // Test if list is empty
    printf("List is empty, should be 0: %d\n", test_list_is_empty(&list));

    failed_test_count += print_all_elements_and_compare(&list, "1 2");

    // Test append left
    int d = 4;
    if (test_list_append_left(&list, &d) == TRUE) {
        printf("Passed append left test.\n");
    } else {
        printf("Failed append left test.\n");
        failed_test_count++;
    }

    failed_test_count += print_all_elements_and_compare(&list, "4 1 2");

    // Clear the list
    test_list_clear(&list);

    // Test if list is empty
    printf("List is empty, should be 1: %d\n", test_list_is_empty(&list));

    print_test_end(test_name, failed_test_count);
    return 0;
}