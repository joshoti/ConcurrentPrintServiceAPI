#ifndef SIGNAL_CATCHER_H
#define SIGNAL_CATCHER_H

#include <pthread.h>
#include <signal.h>

struct TimedQueue;
struct SimulationStatistics;

// --- Utility functions ---
/**
 * @brief Empties the given queue of all jobs, logging each removal and updating statistics.
 * 
 * @param queue Pointer to the TimedQueue representing the job queue to be emptied.
 * @param stats Pointer to the SimulationStatistics struct to update statistics.
 */
void empty_queue_if_terminating(struct TimedQueue* queue, struct SimulationStatistics* stats);

// --- Signal Catching Thread Arguments ---
/**
 * @brief Arguments for the signal catching thread.
 */
typedef struct SignalCatchingThreadArgs {
    sigset_t* signal_set; // Set of signals to wait for
    pthread_mutex_t* job_queue_mutex; // Mutex to protect shared state
    pthread_mutex_t* simulation_state_mutex; // Mutex to protect shared state
    pthread_mutex_t* stats_mutex;
    pthread_cond_t* job_queue_not_empty_cv; // Condition variable to signal other threads
    struct TimedQueue* job_queue; // Pointer to the job queue to be emptied
    struct SimulationStatistics* stats; // Simulation statistics to update
    pthread_t* job_receiver_thread; // Pointer to job receiver thread to cancel
    pthread_t* paper_refiller_thread; // Pointer to paper refiller thread to cancel
    int* all_jobs_arrived; // Flag indicating if all jobs have arrived
} SignalCatchingThreadArgs;

// --- Thread function ---
/**
 * @brief Thread function that waits for a SIGINT signal and initiates termination procedures.
 * 
 * @param arg Pointer to any arguments needed by the thread.
 * @return Always returns NULL.
 */
void* sig_int_catching_thread_func(void* arg);

#endif // SIGNAL_CATCHER_H