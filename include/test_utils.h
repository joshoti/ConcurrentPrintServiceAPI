/**
 * @file test_utils.h
 * @brief Header file for test_utils.c, containing utility functions for testing.
 */

/**
 * @brief Prints the start of a test suite with the test name.
 *
 * @param test_name The name of the test suite.
 */
void print_test_start(char* test_name);

/**
 * @brief Prints the end of a test suite with the total number of failed tests.
 *
 * @param test_name The name of the test suite.
 * @param failed_test_count The total number of failed tests.
 */
void print_test_end(char* test_name, int failed_test_count);