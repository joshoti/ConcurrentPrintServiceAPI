#ifndef PREPROCESSING_H
#define PREPROCESSING_H

/**
 * @file preprocessing.h
 * @brief Header file for preprocessing.c, containing function declarations
 *        and shared variables for command line argument processing and thread management.
 */

typedef struct {
    double job_arrival_time_us;
    double job_printing_time_us;
    int queue_capacity;
    int printer_paper_capacity;
    double refill_time_us;
    int num_jobs;
} SimulationParameters;

/**
 * @brief Print usage information for the program.
 */
void usage();

/**
 * @brief Check if a double value is positive.
 * @param str The name of the parameter being checked (for error messages).
 * @param value The double value to check.
 * @return 1 if the value is positive, 0 otherwise.
 */
int is_positive_double(const char* str, double value);
/**
 * @brief Check if an integer value is positive.
 * @param str The name of the parameter being checked (for error messages).
 * @param value The integer value to check.
 * @return 1 if the value is positive, 0 otherwise.
 */
int is_positive_integer(const char* str, int value);

/**
 * @brief Process command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @param params Pointer to SimulationParameters struct to populate
 * @return 0 on failure, 1 on success
 */
int process_args(int argc, char *argv[], SimulationParameters* params);

#endif // PREPROCESSING_H