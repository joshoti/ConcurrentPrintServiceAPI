#ifndef PAPER_REFILL_H
#define PAPER_REFILL_H

#include <pthread.h>

struct LinkedList;
struct SimulationParameters;
struct SimulationStatistics;

// --- Utility functions ---
/**
 * @brief Prints paper refiller debug information.
 *
 * @param papers_supplied The number of papers supplied during the refill.
 */
void debug_refiller(int papers_supplied);

// --- Paper Refill Thread Arguments ---
/**
 * @brief Arguments for the paper refiller thread.
 */
typedef struct PaperRefillThreadArgs {
    pthread_mutex_t* paper_refill_queue_mutex;
    pthread_mutex_t* stats_mutex;
    pthread_mutex_t* simulation_state_mutex; // protects g_terminate_now
    pthread_cond_t* refill_needed_cv;
    pthread_cond_t* refill_supplier_cv;
    struct LinkedList* paper_refill_queue;
    struct SimulationParameters* params;
    struct SimulationStatistics* stats;
    int* all_jobs_served;
} PaperRefillThreadArgs;

// --- Thread function ---
/**
 * @brief The main function for the paper refiller thread.
 *
 * @param arg Pointer to the PaperRefillThreadArgs struct.
 * @return NULL
 */
void* paper_refill_thread_func(void* arg);

#endif // PAPER_REFILL_H