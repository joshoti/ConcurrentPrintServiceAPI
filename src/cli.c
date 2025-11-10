#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "linked_list.h"
#include "timed_queue.h"
#include "job_receiver.h"
#include "paper_refiller.h"
#include "printer.h"
#include "common.h"
#include "preprocessing.h"
#include "log_router.h"
#include "logger.h"
#include "simulation_stats.h"
#include "signalcatcher.h"

extern int g_debug;
extern int g_terminate_now;

int main(int argc, char *argv[]) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, (sigset_t*)0);

    // --- Thread identifiers ---
    pthread_t printer1_thread;
    pthread_t printer2_thread;
    pthread_t job_receiver_thread;
    pthread_t paper_refill_thread;
    pthread_t signal_catching_thread;

    // --- Synchronization primitives (shared) ---
    pthread_mutex_t job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t paper_refill_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t simulation_state_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t job_queue_not_empty_cv = PTHREAD_COND_INITIALIZER;
    pthread_cond_t refill_needed_cv = PTHREAD_COND_INITIALIZER;
    pthread_cond_t refill_supplier_cv = PTHREAD_COND_INITIALIZER;

    // --- Simulation state ---
    simulation_parameters_t params = SIMULATION_DEFAULT_PARAMS;
    simulation_statistics_t stats = (simulation_statistics_t){0};
    int all_jobs_arrived = 0;
    int all_jobs_served = 0;
    timed_queue_t job_queue;
    linked_list_t paper_refill_queue;
    timed_queue_init(&job_queue);
    list_init(&paper_refill_queue);

    if (!process_args(argc, argv, &params)) return 1;

    // --- Thread argument structs ---
    job_thread_args_t job_receiver_args = {
        .job_queue_mutex = &job_queue_mutex,
        .stats_mutex = &stats_mutex,
        .simulation_state_mutex = &simulation_state_mutex,
        .job_queue_not_empty_cv = &job_queue_not_empty_cv,
        .job_queue = &job_queue,
        .simulation_params = &params,
        .stats = &stats,
        .all_jobs_arrived = &all_jobs_arrived
    };

    // Concrete printer instances
    printer_t printer1 = {.id = 1, .current_paper_count = params.printer_paper_capacity, .capacity = params.printer_paper_capacity, .total_papers_used = 0, .jobs_printed_count = 0};
    printer_t printer2 = {.id = 2, .current_paper_count = params.printer_paper_capacity, .capacity = params.printer_paper_capacity, .total_papers_used = 0, .jobs_printed_count = 0};

    printer_thread_args_t printer1_args = {
        .paper_refill_queue_mutex = &paper_refill_queue_mutex,
        .job_queue_mutex = &job_queue_mutex,
        .stats_mutex = &stats_mutex,
        .simulation_state_mutex = &simulation_state_mutex,
        .job_queue_not_empty_cv = &job_queue_not_empty_cv,
        .refill_needed_cv = &refill_needed_cv,
        .refill_supplier_cv = &refill_supplier_cv,
        .paper_refill_thread = &paper_refill_thread,
        .job_queue = &job_queue,
        .paper_refill_queue = &paper_refill_queue,
        .params = &params,
        .stats = &stats,
        .all_jobs_served = &all_jobs_served,
        .all_jobs_arrived = &all_jobs_arrived,
        .printer = &printer1
    };

    printer_thread_args_t printer2_args = printer1_args;
	printer2_args.printer = &printer2;

    paper_refill_thread_args_t paper_refill_args = {
        .paper_refill_queue_mutex = &paper_refill_queue_mutex,
        .stats_mutex = &stats_mutex,
        .simulation_state_mutex = &simulation_state_mutex,
        .refill_needed_cv = &refill_needed_cv,
        .refill_supplier_cv = &refill_supplier_cv,
        .paper_refill_queue = &paper_refill_queue,
        .params = &params,
        .stats = &stats,
        .all_jobs_served = &all_jobs_served
    };

    signal_catching_thread_args_t signal_catching_args = {
        .signal_set = &set,
        .job_queue_mutex = &job_queue_mutex,
        .simulation_state_mutex = &simulation_state_mutex,
        .paper_refill_queue_mutex = &paper_refill_queue_mutex,
        .stats_mutex = &stats_mutex,
        .job_queue_not_empty_cv = &job_queue_not_empty_cv,
        .refill_needed_cv = &refill_needed_cv,
        .refill_supplier_cv = &refill_supplier_cv,
        .job_queue = &job_queue,
        .stats = &stats,
        .job_receiver_thread = &job_receiver_thread,
        .paper_refill_thread = &paper_refill_thread,
        .all_jobs_arrived = &all_jobs_arrived
    };

    // Bind logger backend for terminal mode
    struct LoggerBackend logs = {
        .log_simulation_parameters = log_simulation_parameters,
        .log_simulation_start = log_simulation_start,
        .log_simulation_end = log_simulation_end,
        .log_system_arrival = log_system_arrival,
        .log_dropped_job = log_dropped_job,
        .log_removed_job = log_removed_job,
        .log_queue_arrival = log_queue_arrival,
        .log_queue_departure = log_queue_departure,
        .log_printer_arrival = log_printer_arrival,
        .log_system_departure = log_system_departure,
        .log_paper_empty = log_paper_empty,
        .log_paper_refill_start = log_paper_refill_start,
        .log_paper_refill_end = log_paper_refill_end,
        .log_ctrl_c_pressed = log_ctrl_c_pressed,
        .log_statistics = log_statistics,
    };
    log_router_set_logger_backend(&logs);
    // Terminal mode: print to stdout
    set_log_mode(LOG_MODE_TERMINAL);
    // --- Start of simulation logging ---
    emit_simulation_parameters(&params);
    emit_simulation_start(&stats);

    // --- Create threads in order ---
    // 1) Job receiver (produces jobs)
    pthread_create(&job_receiver_thread, NULL, job_receiver_thread_func, &job_receiver_args);

    // 2) Paper refiller (services refill requests)
    pthread_create(&paper_refill_thread, NULL, paper_refill_thread_func, &paper_refill_args);

    // 3) Printers (consumers)
    pthread_create(&printer1_thread, NULL, printer_thread_func, &printer1_args);
    pthread_create(&printer2_thread, NULL, printer_thread_func, &printer2_args);

    // 4) Signal catcher (created last, after we have thread IDs to pass by pointer)
    pthread_create(&signal_catching_thread, NULL, sig_int_catching_thread_func, &signal_catching_args);

    // --- Wait for threads to finish ---
    // Join producer first so no new jobs are created
    pthread_join(job_receiver_thread, NULL);
    if (g_debug) printf("job_receiver_thread joined\n");

    // Join the printers
    pthread_join(printer1_thread, NULL);
    if (g_debug) printf("printer1 thread joined\n");
    pthread_join(printer2_thread, NULL);
    if (g_debug) printf("printer2 thread joined\n");

    // Join paper refiller
    pthread_join(paper_refill_thread, NULL);
    if (g_debug) printf("paper_refill_thread joined\n");

    // Signal catcher might still be waiting for SIGINT; cancel and join
    pthread_cancel(signal_catching_thread);
    pthread_join(signal_catching_thread, NULL);
    if (g_debug) printf("signal catching thread joined\n");

    // --- Final logging ---
    emit_simulation_end(&stats);
    emit_statistics(&stats);

    // --- Cleanup synchronization primitives ---
    pthread_mutex_destroy(&job_queue_mutex);
    pthread_mutex_destroy(&paper_refill_queue_mutex);
    pthread_mutex_destroy(&stats_mutex);
    pthread_mutex_destroy(&simulation_state_mutex);
    pthread_cond_destroy(&job_queue_not_empty_cv);
    pthread_cond_destroy(&refill_needed_cv);
    pthread_cond_destroy(&refill_supplier_cv);

    if (g_debug) printf("All threads joined and resources cleaned up.\n");
    return 0;
}