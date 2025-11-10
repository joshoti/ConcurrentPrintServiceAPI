#include <stdio.h>
#include <string.h>

#include "test_utils.h"


void print_test_start(char* test_name) {
    printf("=============================== ");
    printf("[%s] Starting test suite", test_name);
    printf(" ===============================\n");
}

void print_test_end(char* test_name, int failed_test_count) {
    printf("========================================\n");
    printf("[%s] Total failed tests: %d\n", test_name, failed_test_count);
    printf("========================================\n\n\n");
}