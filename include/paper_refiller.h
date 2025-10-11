#ifndef PAPER_REFILLER_H
#define PAPER_REFILLER_H

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

// --- Paper Refiller Thread Arguments ---
/**
 * @brief Arguments for the paper refiller thread.
 */
typedef struct PaperRefillerThreadArgs {
    // TODO: confirm
    pthread_mutex_t* paper_refill_queue_mutex;
    pthread_mutex_t* stats_mutex;
    pthread_mutex_t* simulation_state_mutex; // protects g_terminate_now
    pthread_cond_t* refill_needed_cv;
    struct LinkedList* paper_refill_queue;
    struct SimulationParameters* params;
    struct SimulationStatistics* stats;
} PaperRefillerThreadArgs;

// --- Thread function ---
/**
 * @brief The main function for the paper refiller thread.
 *
 * @param arg Pointer to the PaperRefillerThreadArgs struct.
 * @return NULL
 */
void* paper_refiller_thread_func(void* arg);

#endif // PAPER_REFILLER_H