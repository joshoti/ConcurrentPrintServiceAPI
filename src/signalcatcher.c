#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "log_router.h"
#include "timeutils.h"
#include "job_receiver.h"
#include "linked_list.h"
#include "timed_queue.h"
#include "signalcatcher.h"
#include "simulation_stats.h"

extern int g_debug;
extern int g_terminate_now;

void empty_queue_if_terminating(timed_queue_t* queue, simulation_statistics_t* stats) {
    while (!timed_queue_is_empty(queue)) {
    list_node_t* curr = timed_queue_dequeue_front(queue);
    job_t* job = (job_t*)curr->data;
        job->queue_departure_time_us = get_time_in_us();
        emit_removed_job(job);
        free(curr);
        free(job);
        stats->total_jobs_removed++;
    }
}

void* sig_int_catching_thread_func(void* arg) {
    int sig;
    signal_catching_thread_args_t* args = (signal_catching_thread_args_t*)arg;
    sigwait(args->signal_set, &sig);

    pthread_mutex_lock(args->simulation_state_mutex);
    g_terminate_now = 1;
    *args->all_jobs_arrived = 1;
    pthread_mutex_unlock(args->simulation_state_mutex);

    pthread_mutex_lock(args->stats_mutex);
    emit_simulation_stopped(args->stats);
    pthread_mutex_unlock(args->stats_mutex);
    if (g_debug) printf("Canceling job receiver thread\n");
    if (args->job_receiver_thread) pthread_cancel(*args->job_receiver_thread);
    if (g_debug) printf("Canceling paper refill thread\n");
    if (args->paper_refill_thread) pthread_cancel(*args->paper_refill_thread);
    
    // Lock both mutexes in a defined order to prevent deadlock
    pthread_mutex_lock(args->job_queue_mutex);
    pthread_mutex_lock(args->stats_mutex);

    empty_queue_if_terminating(args->job_queue, args->stats); // empty job queue
    pthread_cond_broadcast(args->job_queue_not_empty_cv); // wake up printer threads to let them exit

    // Unlock in reverse order
    pthread_mutex_unlock(args->stats_mutex);
    pthread_mutex_unlock(args->job_queue_mutex);

    // Wake up any printers or refiller that might be waiting
    pthread_mutex_lock(args->paper_refill_queue_mutex);
    pthread_cond_broadcast(args->refill_needed_cv); // wake up printer threads to let them exit if needed
    pthread_cond_broadcast(args->refill_supplier_cv); // wake up refiller thread to let it exit if needed
    pthread_mutex_unlock(args->paper_refill_queue_mutex);
    if (g_debug) printf("Signal handler exiting\n");

    pthread_exit((void*)1);
}