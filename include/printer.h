#ifndef PRINTER_H
#define PRINTER_H

#include <pthread.h>

struct linked_list;
struct timed_queue;
struct simulation_parameters;
struct simulation_statistics;

// --- Printer structure ---
typedef struct printer {
    int id; // Unique identifier for the printer
    int current_paper_count; // Current number of papers in the printer
    int total_papers_used; // Total number of papers used by this printer
    int capacity; // Maximum paper capacity of the printer
    int jobs_printed_count; // Total number of jobs printed by this printer
} printer_t;

// --- Utility functions ---
/**
 * @brief Prints printer details for debugging purposes.
 * @param printer Pointer to the Printer struct.
 */
void debug_printer(const printer_t* printer);

// --- Printer Thread Arguments ---
/**
 * @brief Arguments for the printer thread.
 */
typedef struct printer_thread_args {
    pthread_mutex_t* paper_refill_queue_mutex;
    pthread_mutex_t* job_queue_mutex;
    pthread_mutex_t* stats_mutex;
    pthread_mutex_t* simulation_state_mutex; // protects g_terminate_now
    pthread_cond_t* job_queue_not_empty_cv;
    pthread_cond_t* refill_needed_cv;
    pthread_cond_t* refill_supplier_cv;
    pthread_t* paper_refill_thread;
    struct timed_queue* job_queue;
    struct linked_list* paper_refill_queue;
    struct simulation_parameters* params;
    struct simulation_statistics* stats;
    int* all_jobs_served;
    int* all_jobs_arrived;
    printer_t* printer;
} printer_thread_args_t;

// --- Thread function ---
/**
 * @brief The main function for the printer thread.
 *
 * @param arg Pointer to the PrinterThreadArgs struct.
 * @return NULL
 */
void* printer_thread_func(void* arg);

#endif // PRINTER_H