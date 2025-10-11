#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "job_receiver.h"
#include "preprocessing.h"
#include "linked_list.h"
#include "timed_queue.h"
#include "timeutils.h"
#include "logger.h"
#include "simulation_stats.h"

extern int g_terminate_now;
extern int g_debug;

int init_job(Job* job, int job_id, int inter_arrival_time_us, int papers_required) {
    if (job == NULL) {
        return FALSE;
    }
    job->id = job_id;
    job->inter_arrival_time_us = inter_arrival_time_us;
    job->papers_required = papers_required;

    // Initialize service time to 0; will be set later based on printing rate
    job->service_time_requested_ms = 0;

    // Initialize timestamps to 0
    job->system_arrival_time_us = 0;
    job->queue_arrival_time_us = 0;
    job->queue_departure_time_us = 0;
    job->service_arrival_time_us = 0;
    job->service_departure_time_us = 0;

    return TRUE;
}

void drop_job_from_system(Job* job, unsigned long previous_job_arrival_time_us) {
    if (job == NULL) return;
    
    // Log the dropped job (this will update statistics internally)
    log_dropped_job(job, previous_job_arrival_time_us, NULL); // stats will be passed by caller
    
    // Free the job memory
    free(job);
}

void debug_job(Job* job) {
    if (job == NULL) {
        printf("Job is NULL\n");
        return;
    }
    
    printf("\nJob Debug Info:\n");
    printf("  Job ID: %d\n", job->id);
    printf("  Inter-arrival time: %d us\n", job->inter_arrival_time_us);
    printf("  Papers required: %d\n", job->papers_required);
    printf("  Service time requested: %d ms\n", job->service_time_requested_ms);
    printf("  System arrival time: %lu us\n", job->system_arrival_time_us);
    printf("  Queue arrival time: %lu us\n", job->queue_arrival_time_us);
    printf("  Queue departure time: %lu us\n", job->queue_departure_time_us);
    printf("  Service arrival time: %lu us\n", job->service_arrival_time_us);
    printf("  Service departure time: %lu us\n", job->service_departure_time_us);
}

void* job_receiver_thread_func(void* arg) {
    JobThreadArgs* args = (JobThreadArgs*)arg;
    if (args == NULL) {
        fprintf(stderr, "Error: JobThreadArgs is NULL\n");
        return NULL;
    }
    
    // Extract arguments
    pthread_mutex_t* job_queue_mutex = args->job_queue_mutex;
    pthread_mutex_t* stats_mutex = args->stats_mutex;
    pthread_mutex_t* simulation_state_mutex = args->simulation_state_mutex;
    pthread_cond_t* job_queue_not_empty_cv = args->job_queue_not_empty_cv;
    TimedQueue* job_queue = args->job_queue;
    SimulationParameters* params = args->simulation_params;
    SimulationStatistics* stats = args->stats;
    int* all_jobs_arrived = args->all_jobs_arrived;
    
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    if (g_debug) printf("Job receiver thread started\n");

    unsigned long previous_job_arrival_time_us = stats->simulation_start_time_us;
    
    for (int job_idx = 0; job_idx < params->num_jobs; job_idx++) {
        int inter_arrival_time_us;
        int papers_required;
        
        inter_arrival_time_us = (int)params->job_arrival_time_us;
        papers_required = random_between(params->papers_required_lower_bound, params->papers_required_upper_bound);

        // Allocate and initialize job
        Job* job = (Job*)malloc(sizeof(Job));
        if (!init_job(job, job_idx + 1, inter_arrival_time_us, papers_required)) {
            fprintf(stderr, "Error: Failed to initialize job %d\n", job_idx + 1);
            free(job);
            continue;
        }
        
        // Sleep for inter-arrival time
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        usleep(inter_arrival_time_us);
        
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        
        // Check for termination signal
        pthread_mutex_lock(simulation_state_mutex);
        if (g_terminate_now) {
            *all_jobs_arrived = 1;
            pthread_mutex_unlock(simulation_state_mutex);
            free(job);
            break;
        }
        pthread_mutex_unlock(simulation_state_mutex);
        
        // Set system arrival time
        job->system_arrival_time_us = get_time_in_us();
        
        // Check if job should be dropped (e.g., if queue is full)
        pthread_mutex_lock(job_queue_mutex);
        
        int queue_length = timed_queue_length(job_queue);
        if (queue_length >= params->queue_capacity) {
            // Drop the job
            pthread_mutex_unlock(job_queue_mutex);
            unsigned long temp_arrival_time_us = job->system_arrival_time_us; // store before freeing
            
            pthread_mutex_lock(stats_mutex);
            drop_job_from_system(job, previous_job_arrival_time_us);
            pthread_mutex_unlock(stats_mutex);

            previous_job_arrival_time_us = temp_arrival_time_us;
            continue;
        }
        
        // Add job to queue
        job->queue_arrival_time_us = get_time_in_us();
        timed_queue_enqueue(job_queue, job);
        
        // Update statistics
        pthread_mutex_lock(stats_mutex);
        stats->max_job_queue_length = 
            (queue_length > stats->max_job_queue_length) ? (queue_length) : stats->max_job_queue_length;
        log_queue_arrival(job, stats, job_queue);
        pthread_mutex_unlock(stats_mutex);
        
        previous_job_arrival_time_us = job->system_arrival_time_us;
        
        // Signal that a job is available
        pthread_cond_signal(job_queue_not_empty_cv);
        pthread_mutex_unlock(job_queue_mutex);
    }
    
    // Mark that all jobs have arrived
    pthread_mutex_lock(simulation_state_mutex);
    *all_jobs_arrived = 1;
    pthread_mutex_unlock(simulation_state_mutex);
    
    // Wake up any waiting threads
    pthread_cond_broadcast(job_queue_not_empty_cv);
    
    return NULL;
}