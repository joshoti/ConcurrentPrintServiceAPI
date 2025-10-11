#ifndef PREPROCESSING_H
#define PREPROCESSING_H

/**
 * @file preprocessing.h
 * @brief Header file for preprocessing.c, containing function declarations
 *        and shared variables for command line argument processing and thread management.
 */

typedef struct SimulationParameters {
    double job_arrival_time_us;
    int papers_required_lower_bound;
    int papers_required_upper_bound;
    int queue_capacity;
    double printing_rate;
    int printer_paper_capacity;
    double refill_rate;
    int num_jobs;
} SimulationParameters;

/**
 * job_arrival_time_us: 300,000 us = 1 job every 0.3 sec
 * papers_required_lower_bound: 20 pages
 * papers_required_upper_bound: 30 pages
 * queue_capacity: 30 jobs
 * printing_rate: 3.3 papers/sec
 * printer_paper_capacity: 200 pages
 * refill_rate: 15 papers/sec
 * num_jobs: 10 jobs
 */
#define SIMULATION_DEFAULT_PARAMS {300000, 20, 30, 30, 3.3, 200, 15, 10}

/**
 * @brief Print usage information for the program.
 */
void usage();

/**
 * @brief Generate a random integer between lower and upper (inclusive).
 * @param lower The lower bound inclusive.
 * @param upper The upper bound inclusive.
 * @return A random integer between lower and upper.
 */
int random_between(int lower, int upper);

/**
 * @brief Swap the values of lower and upper bounds if lower is greater than upper.
 * @param lower Pointer to the lower bound.
 * @param upper Pointer to the upper bound.
 */
void swap_bounds(int* lower, int* upper);

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