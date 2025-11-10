#ifndef SIGNAL_CATCHER_H
#define SIGNAL_CATCHER_H

#include <pthread.h>
#include <signal.h>

struct timed_queue;
struct simulation_statistics;

// --- Utility functions ---
/**
 * @brief Empties the given queue of all jobs, logging each removal and updating statistics.
 * 
 * @param queue Pointer to the TimedQueue representing the job queue to be emptied.
 * @param stats Pointer to the SimulationStatistics struct to update statistics.
 */
void empty_queue_if_terminating(struct timed_queue* queue, struct simulation_statistics* stats);

// --- Signal Catching Thread Arguments ---
/**
 * @brief Arguments for the signal catching thread.
 */
typedef struct signal_catching_thread_args {
    sigset_t* signal_set; // Set of signals to wait for
    pthread_mutex_t* job_queue_mutex; // Mutex to protect shared state
    pthread_mutex_t* simulation_state_mutex; // Mutex to protect shared state
    pthread_mutex_t* paper_refill_queue_mutex; // Mutex to protect paper refill queue
    pthread_mutex_t* stats_mutex; // Mutex to protect statistics data structure
    pthread_cond_t* job_queue_not_empty_cv; // Condition variable to signal printer threads
    pthread_cond_t* refill_needed_cv; // Condition variable to signal printers waiting for paper
    pthread_cond_t* refill_supplier_cv; // Condition variable to signal paper refill thread
    struct timed_queue* job_queue; // Pointer to the job queue to be emptied
    struct simulation_statistics* stats; // Simulation statistics to update
    pthread_t* job_receiver_thread; // Pointer to job receiver thread to cancel
    pthread_t* paper_refill_thread; // Pointer to paper refill thread to cancel
    int* all_jobs_arrived; // Flag indicating if all jobs have arrived
} signal_catching_thread_args_t;

// --- Thread function ---
/**
 * @brief Thread function that waits for a SIGINT signal and initiates termination procedures.
 * 
 * @param arg Pointer to any arguments needed by the thread.
 * @return Always returns NULL.
 */
void* sig_int_catching_thread_func(void* arg);

#endif // SIGNAL_CATCHER_H