#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "logger.h"
#include "timeutils.h"
#include "job_receiver.h"
#include "linked_list.h"
#include "timed_queue.h"
#include "signalcatcher.h"
#include "simulation_stats.h"

extern int g_debug;
extern int g_terminate_now;

void empty_queue_if_terminating(TimedQueue* queue, SimulationStatistics* stats) {
    while (!timed_queue_is_empty(queue)) {
        ListNode* curr = timed_queue_dequeue_front(queue);
        Job* job = (Job*)curr->data;
        job->queue_departure_time_us = get_time_in_us();
        log_removed_job(job);
        free(curr);
        free(job);
        stats->total_jobs_removed++;
    }
}


void* sig_int_catching_thread_func(void* arg) {
    int sig;
    SignalCatchingThreadArgs* args = (SignalCatchingThreadArgs*)arg;
    sigwait(args->signal_set, &sig);

    pthread_mutex_lock(args->simulation_state_mutex);
    g_terminate_now = 1;
    *args->all_jobs_arrived = 1;
    pthread_mutex_unlock(args->simulation_state_mutex);

    pthread_mutex_lock(args->stats_mutex);
    log_ctrl_c_pressed(args->stats);
    pthread_mutex_unlock(args->stats_mutex);
    if (g_debug) printf("Canceling job receiver thread\n");
    pthread_cancel(args->job_receiver_thread);
    
    // Lock both mutexes in a defined order to prevent deadlock
    pthread_mutex_lock(args->job_queue_mutex);
    pthread_mutex_lock(args->stats_mutex);

    empty_queue_if_terminating(args->job_queue, args->stats);
    pthread_cond_broadcast(args->job_queue_not_empty_cv);

    // Unlock in reverse order
    pthread_mutex_unlock(args->stats_mutex);
    pthread_mutex_unlock(args->job_queue_mutex);
    if (g_debug) printf("Signal handler exiting\n");

    pthread_exit((void*)1);
}