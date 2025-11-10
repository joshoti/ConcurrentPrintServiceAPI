#ifndef JOB_RECEIVER_H
#define JOB_RECEIVER_H

# include <pthread.h>

struct timed_queue;
struct simulation_parameters;
struct simulation_statistics;

// --- Job structure ---
typedef struct job {
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
} job_t;

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
int init_job(job_t* job, int job_id, int inter_arrival_time_us, int papers_required);

/**
 * @brief Drop a job from the system. Updates statistics accordingly. Free the job memory after dropping.
 * @param job Pointer to the Job struct to drop.
 * @param previous_job_arrival_time_us Arrival time of the previous job in microseconds.
 * @param stats Pointer to the simulation_statistics struct to update.
 */
void drop_job_from_system(job_t* job, unsigned long previous_job_arrival_time_us, struct simulation_statistics* stats);
/**
 * @brief Prints job details for debugging purposes.
 * @param job Pointer to the Job struct to print.
 */
void debug_job(job_t* job);

// --- Job Receiver Thread Arguments ---
/**
 * @brief Arguments for the job receiver thread.
 */
typedef struct job_thread_args {
    pthread_mutex_t* job_queue_mutex;
    pthread_mutex_t* stats_mutex;
    pthread_mutex_t* simulation_state_mutex; // protects all_jobs_arrived and g_terminate_now
    pthread_cond_t* job_queue_not_empty_cv;
    struct timed_queue* job_queue;
    struct simulation_parameters* simulation_params;
    struct simulation_statistics* stats;
    int* all_jobs_arrived;
} job_thread_args_t;

// --- Thread function ---
/**
 * @brief Function executed by the job receiver thread.
 *
 * @param arg Pointer to JobThreadArgs struct.
 * @return NULL
 */
void* job_receiver_thread_func(void* arg);

#endif // JOB_RECEIVER_H