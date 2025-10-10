#ifndef SIGNAL_CATCHER_H
#define SIGNAL_CATCHER_H

typedef struct LinkedList;
typedef struct SimulationStatistics;

// --- Utility functions ---
/**
 * @brief Empties the given queue of all jobs, logging each removal and updating statistics.
 * 
 * @param queue Pointer to the LinkedList representing the job queue to be emptied.
 * @param stats Pointer to the SimulationStatistics struct to update statistics.
 */
void empty_queue_if_terminating(LinkedList* queue, SimulationStatistics* stats);

// --- Thread function ---
/**
 * @brief Thread function that waits for a SIGINT signal and initiates termination procedures.
 * 
 * @param arg Pointer to any arguments needed by the thread.
 * @return Always returns NULL.
 */
void* sig_int_catching_thread_func(void* arg);

#endif // SIGNAL_CATCHER_H