#ifndef JOB_RECEIVER_H
#define JOB_RECEIVER_H

// --- Job structure ---
typedef struct {
    // --- Job Attributes ---
    int job_id;
    int inter_arrival_time_us; // time between this packet and the previous packet
    int papers_required; // number of papers required by the job
    int service_time_requested_us; // time required to service the job
    
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
 * @param service_time_requested_us Time required to service the job in microseconds.
 * @return 1 on success, 0 on failure (e.g., invalid parameters).
 */
int init_job(Job* job, int job_id, int inter_arrival_time_us,
    int papers_required, int service_time_requested_us);

/**
 * @brief Retrieves job information from random generator or the input file.
 * @param inter_arrival_time_us Pointer to store the inter-arrival time.
 * @param papers_required Pointer to store the number of papers required.
 * @param service_time_requested_us Pointer to store the service time requested.
 * @param line_num The line number in the input file to read the job from.
 */
void get_job_info(int* inter_arrival_time_us, int* papers_required,
    int* service_time_requested_us, int line_num);
/**
 * @brief Simulates dropping a job from the system. Updates statistics accordingly.
 * @param job Pointer to the Job struct to drop.
 * @param previous_job_arrival_time_us Arrival time of the previous job in microseconds.
 * @param total_jobs_dropped Pointer to the counter for total jobs dropped.
 */
void drop_job_from_system(Job* job,
    unsigned long previous_job_arrival_time_us, double* total_jobs_dropped);
/**
 * @brief Prints job details for debugging purposes.
 * @param job Pointer to the Job struct to print.
 */
void debug_job(Job* job);

// --- Job Receiver Thread Arguments ---
/**
 * @brief Arguments for the job receiver thread.
 */
typedef struct {
} JobArguments;

// --- Thread function ---
/**
 * @brief Function executed by the job receiver thread.
 *
 * @param arg Pointer to JobArguments struct.
 * @return Always returns NULL.
 */
void* job_receiver_thread_func(void* arg);

#endif // JOB_RECEIVER_H