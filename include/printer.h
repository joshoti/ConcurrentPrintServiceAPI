#ifndef PRINTER_H
#define PRINTER_H

// --- Printer structure ---
typedef struct Printer {
    int id; // Unique identifier for the printer
    int paper_count; // Current number of papers in the printer
    int capacity; // Maximum paper capacity of the printer
    int printed_count; // Total number of jobs printed by this printer
} Printer;

// --- Utility functions ---
void debug_printer(int printer_id, int printed_count);

// --- Printer Thread Arguments ---
/**
 * @brief Arguments for the printer thread.
 */
typedef struct PrinterThreadArgs {
    // TODO: confirm
    pthread_mutex_t* printer_mutex;
    pthread_mutex_t* job_queue_mutex;
    pthread_mutex_t* stats_mutex;
    pthread_mutex_t* simulation_state_mutex; // protects terminate_now
    pthread_cond_t* job_queue_not_empty_cv;
    pthread_cond_t* printer_paper_empty_cv;
    struct LinkedList* job_queue;
    struct SimulationParameters* params;
    struct SimulationStatistics* stats;
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