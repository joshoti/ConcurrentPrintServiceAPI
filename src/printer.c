#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "preprocessing.h"
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
        if (terminate || is_exit_condition_met(*(args->all_jobs_arrived), args->job_queue)) {
            if (g_debug) printf("Printer %d is terminating or finished\n", args->printer->id);
            pthread_mutex_unlock(args->job_queue_mutex);
            break;
        }
        // --- Process job ---
        // Check if there are enough papers
        /*
        Consider case of when printer sees job, goes to refill, but while waiting another printer services
        the job and takes it from the queue. And the case where that was the last job in the system.
        */
        ListNode* elem = timed_queue_first(args->job_queue);
        Job* job_to_dequeue = (Job*)elem->data;
        if (job_to_dequeue->papers_required > args->printer->current_paper_count) {
            // Not enough paper for the job at the front of the queue
            pthread_mutex_unlock(args->job_queue_mutex);
            pthread_mutex_lock(args->paper_refill_queue_mutex);
            unsigned long refill_start_time_us = get_time_in_us();
            log_paper_empty(args->printer, job_to_dequeue->id, refill_start_time_us);
            list_append(args->paper_refill_queue, args->printer);
            pthread_cond_broadcast(args->refill_done_cv); // Notify refill thread

            // Wait until paper is refilled
            pthread_cond_wait(args->refill_needed_cv, args->paper_refill_queue_mutex);
            pthread_mutex_unlock(args->paper_refill_queue_mutex);

            // Update stats for paper empty duration
            pthread_mutex_lock(args->stats_mutex);
            int paper_empty_duration_us = get_time_in_us() - refill_start_time_us;
            if (args->printer->id == 1) {
                args->stats->printer1_paper_empty_time_us +=
                    paper_empty_duration_us; // stats: total time printer 1 was idle due to no paper
            } else if (args->printer->id == 2) {
                args->stats->printer2_paper_empty_time_us +=
                    paper_empty_duration_us; // stats: total time printer 2 was idle due to no paper
            }
            pthread_mutex_unlock(args->stats_mutex);
            continue;
        }

        // Get the next job from the queue
        elem = timed_queue_dequeue_front(args->job_queue);
        Job* job = (Job*)elem->data;
        job->queue_departure_time_us = get_time_in_us();
        log_queue_departure(job, args->stats, args->job_queue);

        pthread_mutex_unlock(args->job_queue_mutex);

        // Update job service_time_requested_ms based on printer speed if needed
        job->service_time_requested_ms =
                (int)((job->papers_required / args->params->printing_rate) * 1000); // in ms

        // Log job arrival at printer
        job->service_arrival_time_us = get_time_in_us();
        log_printer_arrival(job, args->printer);

        // Service the job
        usleep(job->service_time_requested_ms * 1000); // Convert ms to us
        args->printer->current_paper_count -= job->papers_required;
        args->printer->total_papers_used += job->papers_required;

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

        // Check exit condition.
        pthread_mutex_lock(args->simulation_state_mutex);
        int have_all_jobs_arrived = *(args->all_jobs_arrived);
        pthread_mutex_unlock(args->simulation_state_mutex);
        pthread_mutex_lock(args->job_queue_mutex);
        if (is_exit_condition_met(have_all_jobs_arrived, args->job_queue)) {
            pthread_mutex_unlock(args->job_queue_mutex);
            if (g_debug) printf("Printer %d has finished\n", args->printer->id);
            break;
        }
        pthread_mutex_unlock(args->job_queue_mutex);

        if (g_debug) printf("Printer %d is looking for next job\n", args->printer->id);
        if (g_debug) debug_printer(args->printer);
    }

    pthread_mutex_lock(args->simulation_state_mutex);
    *(args->all_jobs_served) = 1;
    pthread_mutex_unlock(args->simulation_state_mutex);

    pthread_mutex_lock(args->paper_refill_queue_mutex);
    pthread_cond_broadcast(args->refill_needed_cv); // Notify printer thread in case it's waiting
    pthread_cond_broadcast(args->refill_done_cv); // Notify refill thread in case it's waiting
    pthread_mutex_unlock(args->paper_refill_queue_mutex);
    if (g_debug) printf("Printer %d gracefully exited\n", args->printer->id);
    return NULL;
}