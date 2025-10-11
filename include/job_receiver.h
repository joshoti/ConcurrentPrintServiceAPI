#ifndef JOB_RECEIVER_H
#define JOB_RECEIVER_H

# include <pthread.h>

struct LinkedList;
struct SimulationParameters;
struct SimulationStatistics;

// --- Job structure ---
typedef struct Job {
    // --- Job Attributes ---
    int id;
    int inter_arrival_time_us; // time between this job and the previous job
    int papers_required; // number of papers required by the job
    
    // --- Service Attributes ---
    int service_time_requested_ms; // time required to service the job depending on papers required

    // --- Timestamps for tracking job lifecycle ---
    unsigned long system_arrival_time_us; // time job arrived to the system
    unsigned long queue_arrival_time_us; // time job entered service queue
    unsigned long queue_departure_time_us; // time job left service queue
    unsigned long service_arrival_time_us; // time job started being serviced
    unsigned long service_departure_time_us; // time job left the system
} Job;

// --- Utility functions ---
/**
 * @brief Initializes a Job struct with given parameters.
 *
 * @param job Pointer to the Job struct to initialize.
 * @param job_id Unique identifier for the job.
 * @param inter_arrival_time_us Time between this job and the previous job in microseconds.
 * @param papers_required Number of papers required by the job.
 * @return 1 on success, 0 on failure (e.g., invalid parameters).
 */
int init_job(Job* job, int job_id, int inter_arrival_time_us, int papers_required);

/**
 * @brief Simulates dropping a job from the system. Updates statistics accordingly.
 * @param job Pointer to the Job struct to drop.
 * @param previous_job_arrival_time_us Arrival time of the previous job in microseconds.
 */
void drop_job_from_system(Job* job, unsigned long previous_job_arrival_time_us);
/**
 * @brief Prints job details for debugging purposes.
 * @param job Pointer to the Job struct to print.
 */
void debug_job(Job* job);

// --- Job Receiver Thread Arguments ---
/**
 * @brief Arguments for the job receiver thread.
 */
typedef struct JobThreadArgs {
    pthread_mutex_t* job_queue_mutex;
    pthread_mutex_t* stats_mutex;
    pthread_mutex_t* simulation_state_mutex; // protects all_jobs_arrived and g_terminate_now
    pthread_cond_t* job_queue_not_empty_cv;
    struct LinkedList* job_queue;
    struct SimulationParameters* simulation_params;
    struct SimulationStatistics* stats;
    int* all_jobs_arrived;
} JobThreadArgs;

// --- Thread function ---
/**
 * @brief Function executed by the job receiver thread.
 *
 * @param arg Pointer to JobThreadArgs struct.
 * @return NULL
 */
void* job_receiver_thread_func(void* arg);

#endif // JOB_RECEIVER_H