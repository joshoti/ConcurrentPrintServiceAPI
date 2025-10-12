#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "timeutils.h"
#include "logger.h"
#include "simulation_stats.h"
#include "linked_list.h"
#include "timed_queue.h"
#include "job_receiver.h"
#include "printer.h"

extern int g_debug;
extern int g_terminate_now;

/**
 * @brief Checks if the exit condition for the server thread is met.
 * 
 * @return TRUE if exit condition is met, FALSE otherwise.
 */
static int is_exit_condition_met(int all_jobs_arrived, TimedQueue* job_queue) {
    return all_jobs_arrived && timed_queue_is_empty(job_queue);
}

void debug_printer(const Printer* printer) {
    printf("Debug: Printer %d has printed %d jobs and used %d papers\n",
        printer->id, printer->jobs_printed_count, printer->total_papers_used);
}

void* printer_thread_func(void* arg) {
    PrinterThreadArgs* args = (PrinterThreadArgs*)arg;

    if (g_debug) printf("Printer %d thread started\n", args->printer->id);

    while (1) {
        pthread_mutex_lock(args->simulation_state_mutex);
        int terminate = g_terminate_now;
        pthread_mutex_unlock(args->simulation_state_mutex);

        pthread_mutex_lock(args->job_queue_mutex);
        while (timed_queue_is_empty(args->job_queue) && !terminate) {
            pthread_cond_wait(args->job_queue_not_empty_cv, args->job_queue_mutex);
            if (terminate || is_exit_condition_met(*(args->all_jobs_arrived), args->job_queue)) break;
        }
        if (terminate) {
            // empty job queue if service is terminating
            while (!timed_queue_is_empty(args->job_queue)) {
                ListNode* elem = timed_queue_dequeue_front(args->job_queue);
                Job* job = (Job*)elem->data;
                job->queue_departure_time_us = get_time_in_us();
                log_removed_job(job);
                args->stats->total_jobs_removed++;
                free(elem);
                free(job);
            }
            if (g_debug) printf("Printer %d is terminating\n", args->printer->id);
            pthread_mutex_unlock(args->job_queue_mutex);
            break;
        }
        if (is_exit_condition_met(*(args->all_jobs_arrived), args->job_queue)) {
            pthread_mutex_unlock(args->job_queue_mutex);
            if (g_debug) printf("Printer %d has finished\n", args->printer->id);
            break;
        }
        // --- Process job ---
        // Check if there are enough papers
        // CHECK BELOW
        ListNode* printer_elem = NULL;
        pthread_mutex_lock(args->paper_refill_queue_mutex);
        if (args->printer->current_paper_count <= 0) {
            // Request paper refill
            if (g_debug) printf("Printer %d is out of paper and requesting refill\n", args->printer->id);
            list_append(args->paper_refill_queue, args->printer);
            pthread_cond_signal(args->refill_needed_cv); // Notify refill thread
            // Wait until paper is refilled
            while (args->printer->current_paper_count <= 0) {
                pthread_cond_wait(args->refill_needed_cv, args->paper_refill_queue_mutex);
                // Safely check the termination flag
                pthread_mutex_lock(args->simulation_state_mutex);
                int terminate_now = g_terminate_now;
                pthread_mutex_unlock(args->simulation_state_mutex);
                if (terminate_now) {
                    if (g_debug) printf("Printer %d is terminating while waiting for paper\n", args->printer->id);
                    pthread_cond_broadcast(args->job_queue_not_empty_cv); // wake up server threads to let them exit if needed
                    pthread_mutex_unlock(args->paper_refill_queue_mutex);
                    pthread_mutex_unlock(args->job_queue_mutex);
                    return NULL;
                }
                if (is_exit_condition_met(*(args->all_jobs_arrived), args->job_queue)) {
                    pthread_mutex_unlock(args->paper_refill_queue_mutex);
                    pthread_mutex_unlock(args->job_queue_mutex);
                    if (g_debug) printf("Printer %d has finished\n", args->printer->id);
                    return NULL;
                }
            }
        }
        pthread_mutex_unlock(args->paper_refill_queue_mutex);
        // CHECK ABOVE
        // Get the next job from the queue
        ListNode* elem = timed_queue_dequeue_front(args->job_queue);
        Job* job = (Job*)elem->data;
        job->queue_departure_time_us = get_time_in_us();
        log_queue_departure(job, args->stats, args->job_queue);

        pthread_mutex_unlock(args->job_queue_mutex);

        // Log job arrival at printer
        job->service_arrival_time_us = get_time_in_us();
        log_printer_arrival(job, args->printer);

        // Service the job
        usleep(job->service_time_requested_ms * 1000); // Convert ms to us
        args->printer->current_paper_count -= job->papers_required;

        // Update job departure time
        job->service_departure_time_us = get_time_in_us();

        // Update stats
        pthread_mutex_lock(args->stats_mutex);
        args->printer->jobs_printed_count++;
        // Log job departure from system and update stats
        log_system_departure(job, args->printer, args->stats);
        pthread_mutex_unlock(args->stats_mutex);

        // Free job resources
        free(elem);
        free(job);

        if (is_exit_condition_met(*(args->all_jobs_arrived), args->job_queue)) {
            if (g_debug) printf("Printer %d has finished\n", args->printer->id);
            break;
        }

        if (g_debug) printf("Printer %d is looking for next job\n", args->printer->id);
        if (g_debug) debug_printer(args->printer);
    }

    pthread_mutex_lock(args->simulation_state_mutex);
    *(args->all_jobs_served) = 1;
    pthread_mutex_unlock(args->simulation_state_mutex);

    pthread_mutex_lock(args->paper_refill_queue_mutex);
    pthread_cond_broadcast(args->refill_needed_cv); // Notify refill thread in case it's waiting
    pthread_mutex_unlock(args->paper_refill_queue_mutex);
    if (g_debug) printf("Printer %d has exited\n", args->printer->id);
    return NULL;
}