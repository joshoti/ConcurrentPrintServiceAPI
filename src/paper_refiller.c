#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "paper_refiller.h"
#include "common.h"
#include "timeutils.h"
#include "printer.h"
#include "linked_list.h"
#include "preprocessing.h"
#include "logger.h"
#include "simulation_stats.h"

extern int g_debug;
extern int g_terminate_now;

void debug_refiller(int papers_supplied) {
    printf("Debug: Paper Refiller supplied %d papers\n", papers_supplied);
}

/**
 * @brief Checks if the exit condition for the paper refiller thread is met.
 * 
 * @param refill_needed_cv Pointer to the condition variable to signal threads.
 * @param paper_refill_queue_mutex Pointer to the mutex protecting the paper refill queue.
 * @return TRUE if exit condition is met, FALSE otherwise.
 */
static int is_exit_condition_met(pthread_cond_t* refill_needed_cv,
    pthread_mutex_t* paper_refill_queue_mutex, int all_jobs_served)
{
    if (all_jobs_served) {
        if (g_debug) printf("Paper refiller thread has finished\n");
        pthread_mutex_lock(paper_refill_queue_mutex);
        pthread_cond_broadcast(refill_needed_cv); // wake up printer threads to let them exit if needed
        pthread_mutex_unlock(paper_refill_queue_mutex);
        return TRUE;
    }
    return FALSE;
}

void* paper_refiller_thread_func(void* arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    
    PaperRefillerThreadArgs* args = (PaperRefillerThreadArgs*)arg;

    if (g_debug) printf("Paper refiller thread started\n");
    while (1) {
        pthread_mutex_lock(args->paper_refill_queue_mutex);

        // Enable cancellation just before the cancellation point to avoid deadlock
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_cond_wait(args->refill_needed_cv, args->paper_refill_queue_mutex); // wait until signaled to refill paper
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

        // Safely check the termination flag
        pthread_mutex_lock(args->simulation_state_mutex);
        int terminate_now = g_terminate_now;
        pthread_mutex_unlock(args->simulation_state_mutex);

        if (terminate_now) {
            if (g_debug) printf("Paper refiller thread signaled to terminate\n");
            pthread_cond_broadcast(args->refill_needed_cv); // wake up server threads to let them exit if needed
            pthread_mutex_unlock(args->paper_refill_queue_mutex);
            return NULL;
        }

        while (!list_is_empty(args->paper_refill_queue)) {
            unsigned long refill_start_time_us = get_time_in_us();
            
            ListNode* elem = list_pop_left(args->paper_refill_queue);
            Printer* printer = (Printer*)elem->data;
            pthread_mutex_unlock(args->paper_refill_queue_mutex); // unlock while refilling

            // Refill paper
            int papers_needed = printer->capacity - printer->current_paper_count;
            if (papers_needed <= 0) {
                if (g_debug) printf("Debug: Paper Refiller found printer %d already full\n", printer->id);
            }

            int time_to_refill_us = (unsigned long)((papers_needed / args->params->refill_rate) * 1000000);
            int time_to_refill_ms = time_to_refill_us / 1000; // for logging and publishing
            log_paper_refill_start(printer, papers_needed, time_to_refill_ms, refill_start_time_us);
            
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            usleep(time_to_refill_us);
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            
            unsigned long refill_end_time_us = get_time_in_us();
            int actual_refill_time_ms = (refill_end_time_us - refill_start_time_us) / 1000;
            log_paper_refill_end(printer, actual_refill_time_ms, refill_end_time_us);

            // Done refilling
            printer->current_paper_count += papers_needed;
            pthread_mutex_lock(args->stats_mutex);
            args->stats->papers_refilled += papers_needed;
            args->stats->total_refill_service_time_us += refill_end_time_us - refill_start_time_us;
            args->stats->paper_refill_events++;
            pthread_mutex_unlock(args->stats_mutex);
            free(elem);
            if (g_debug) debug_refiller(papers_needed);
        }

        if (is_exit_condition_met(args->refill_needed_cv, args->paper_refill_queue_mutex, *(args->all_jobs_served))) return NULL;
    }
    return NULL;
}