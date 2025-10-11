#ifndef PRINTER_H
#define PRINTER_H

#include <pthread.h>

struct LinkedList;
struct TimedQueue;
struct SimulationParameters;
struct SimulationStatistics;

// --- Printer structure ---
typedef struct Printer {
    int id; // Unique identifier for the printer
    int paper_count; // Current number of papers in the printer
    int capacity; // Maximum paper capacity of the printer
    int printed_count; // Total number of jobs printed by this printer
} Printer;

// --- Utility functions ---
/**
 * @brief Prints printer details for debugging purposes.
 * @param printer_id Unique identifier for the printer.
 * @param printed_count Total number of jobs printed by this printer.
 */
void debug_printer(int printer_id, int printed_count);

// --- Printer Thread Arguments ---
/**
 * @brief Arguments for the printer thread.
 */
typedef struct PrinterThreadArgs {
    // TODO: use printer_mutex?
    // pthread_mutex_t* printer_mutex;
    pthread_mutex_t* paper_refill_queue_mutex;
    pthread_mutex_t* job_queue_mutex;
    pthread_mutex_t* stats_mutex;
    pthread_mutex_t* simulation_state_mutex; // protects g_terminate_now
    pthread_cond_t* job_queue_not_empty_cv;
    pthread_cond_t* refill_needed_cv;
    struct TimedQueue* job_queue;
    struct LinkedList* paper_refill_queue;
    struct SimulationParameters* params;
    struct SimulationStatistics* stats;
    int* all_jobs_served;
    int* all_jobs_arrived;
    Printer* printer;
} PrinterThreadArgs;

// --- Thread function ---
/**
 * @brief The main function for the printer thread.
 *
 * @param arg Pointer to the PrinterThreadArgs struct.
 * @return NULL
 */
void* printer_thread_func(void* arg);

#endif // PRINTER_H