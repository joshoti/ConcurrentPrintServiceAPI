#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "preprocessing.h"
#include "job_receiver.h"
#include "printer.h"
#include "simulation_stats.h"
#include "logger.h"
#include "timed_queue.h"
#include "timeutils.h"

static unsigned long reference_time_us = 0;
static unsigned long reference_end_time_us = 0;

/**
 * @brief Logs the given time in milliseconds (ms) relative to the given reference
 * time.
 * 
 * @param time_us The time to log, in microseconds (us).
 * @param reference_time_us The reference time to log against, in microseconds (us).
 *
 * Sample output called 250ms (250,000us) after reference time is "00000251.457ms: "
 */
static void log_time(unsigned long time_us, unsigned long reference_time_us) {
    time_us -= reference_time_us;

    int milliseconds = 0;
    int microseconds = 0;
    time_in_us_to_ms(time_us, &milliseconds, &microseconds);
    printf(time_format, milliseconds, microseconds);
}

void log_simulation_parameters(const SimulationParameters* params) {
    flockfile(stdout);
    printf("================= Simulation parameters: =================\n");
    printf("  Number of jobs: %d\n", params->num_jobs);
    printf("  Job arrival time: %.6g us\n", params->job_arrival_time_us);
    printf("  Printing rate: %.6g\n", params->printing_rate);
    printf("  Printer paper capacity: %d\n", params->printer_paper_capacity);
    printf("  Queue capacity: %d\n", params->queue_capacity);
    printf("  Refill rate: %.6g\n", params->refill_rate);
    printf("  Papers required (lower bound): %d\n", params->papers_required_lower_bound);
    printf("  Papers required (upper bound): %d\n", params->papers_required_upper_bound);
    funlockfile(stdout);
}

void log_simulation_start(SimulationStatistics* stats) {
    reference_time_us = get_time_in_us();
    stats->simulation_start_time_us = reference_time_us;
    flockfile(stdout);
    log_time(reference_time_us, reference_time_us);
    printf("simulation begins\n");
    funlockfile(stdout);
}

void log_simulation_end(SimulationStatistics* stats) {
    reference_end_time_us = get_time_in_us();
    flockfile(stdout);
    log_time(reference_end_time_us, reference_time_us);
    stats->simulation_duration_us = reference_end_time_us - reference_time_us;
    printf("simulation ends, duration = %d.%03dms\n",
        (int)(stats->simulation_duration_us / 1000), (int)(stats->simulation_duration_us % 1000));
    funlockfile(stdout);
}

/**
 * @brief Logs an event when a new job is created in the system or when a job is dropped
 * 
 * @param job_id The id of the job that has been created or dropped.
 * @param papers_required The number of papers required by the job.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds
 * @param current_job_arrival_time_us The current simulation time in microseconds.
 * @param is_dropped Whether the job was dropped (TRUE) or created (FALSE).
 * @param stats The simulation statistics to update.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
static void job_arrival_helper(int job_id, int papers_required,
    unsigned long previous_job_arrival_time_us, unsigned long current_job_arrival_time_us,
    int is_dropped, SimulationStatistics* stats)
{
    flockfile(stdout);
    log_time(current_job_arrival_time_us, reference_time_us);

    int inter_arrival_time_us = current_job_arrival_time_us - previous_job_arrival_time_us;
    stats->total_inter_arrival_time_us += inter_arrival_time_us; // stats: avg job inter-arrival time
    stats->total_jobs_arrived ++; // stats: total jobs arrived
    int time_in_ms = inter_arrival_time_us / 1000;
    int time_in_us = inter_arrival_time_us % 1000;
    printf("job%d arrives, needs %d paper%s, inter-arrival time = %d.%03dms%s\n",
        job_id, papers_required,
        papers_required == 1 ? "" : "s", time_in_ms, time_in_us,
        is_dropped ? ", dropped" : "");
    funlockfile(stdout);
}

void log_system_arrival(Job* job, unsigned long previous_job_arrival_time_us,
    SimulationStatistics* stats) {
    job_arrival_helper(job->id, job->papers_required,
        previous_job_arrival_time_us, job->system_arrival_time_us, FALSE, stats);
}

void log_dropped_job(Job* job, unsigned long previous_job_arrival_time_us,
    SimulationStatistics* stats) {
    stats->total_jobs_dropped += 1;
    job_arrival_helper(job->id, job->papers_required,
        previous_job_arrival_time_us, job->system_arrival_time_us, TRUE, stats);
}

void log_removed_job(Job* job) {
    unsigned long current_time_us = get_time_in_us();
    flockfile(stdout);
    log_time(current_time_us, reference_time_us);
    printf("job%d removed from system\n", job->id);
    funlockfile(stdout);
}

void log_queue_arrival(const Job* job, SimulationStatistics* stats, TimedQueue* job_queue) {
    stats->area_num_in_job_queue_us +=
        (job->queue_arrival_time_us - job_queue->last_interaction_time_us) * // duration of previous state
        (timed_queue_length(job_queue) - 1); // -1 for the job that just entered the queue
    // stats: avg job queue length
    job_queue->last_interaction_time_us = job->queue_arrival_time_us;

    flockfile(stdout);
    log_time(job->queue_arrival_time_us, reference_time_us);
    printf("job%d enters queue, queue length = %d\n", job->id,
        timed_queue_length(job_queue));
    funlockfile(stdout);
}

void log_queue_departure(const Job* job, SimulationStatistics* stats,
    TimedQueue* job_queue)
{
    stats->area_num_in_job_queue_us +=
        (job->queue_departure_time_us - job_queue->last_interaction_time_us) * // duration of previous state
        (timed_queue_length(job_queue) + 1); // +1 for the job that just left the queue
    // stats: avg job queue length
    job_queue->last_interaction_time_us = job->queue_departure_time_us;

    flockfile(stdout);
    log_time(job->queue_departure_time_us, reference_time_us);
    int queue_duration = job->queue_departure_time_us - job->queue_arrival_time_us;
    int time_ms = queue_duration / 1000;
    int time_us = queue_duration % 1000;
    printf("job%d leaves queue, time in queue = %d.%03dms, queue_length = %d\n",
        job->id, time_ms, time_us, timed_queue_length(job_queue));
    funlockfile(stdout);
}

void log_printer_arrival(const Job* job, const Printer* printer) {
    flockfile(stdout);
    log_time(job->service_arrival_time_us, reference_time_us);
    printf("job%d begins service at printer%d, printing %d pages in about %dms\n",
        job->id, printer->id, job->papers_required, job->service_time_requested_ms);
    funlockfile(stdout);
}

void log_system_departure(const Job* job, const Printer* printer,
    SimulationStatistics* stats) {
    flockfile(stdout);
    log_time(job->service_departure_time_us, reference_time_us);

    int system_time = job->service_departure_time_us - job->system_arrival_time_us;
    stats->total_system_time_us += system_time; // stats: avg job system time
    stats->sum_of_system_time_squared_us2 += system_time * system_time; // stats: stddev job system time
    stats->total_jobs_served += 1; // stats: total jobs served
    
    int service_duration = job->service_departure_time_us - job->service_arrival_time_us;
    if (printer->id == 1) {
        stats->total_service_time_p1_us += service_duration; // stats: avg job service time
        stats->jobs_served_by_printer1 += 1; // stats: total jobs served by printer 1
    } else if (printer->id == 2) {
        stats->total_service_time_p2_us += service_duration; // stats: avg job service time
        stats->jobs_served_by_printer2 += 1; // stats: total jobs served by printer 2
    }
    stats->total_queue_wait_time_us +=
        (job->queue_departure_time_us - job->queue_arrival_time_us); // stats: avg job queue wait time

    int time_ms = service_duration / 1000;
    int time_us = service_duration % 1000;
    printf("job%d departs from printer%d, service time = %d.%03dms\n",
        job->id, printer->id, time_ms, time_us);
    funlockfile(stdout);
}

void log_paper_empty(Printer* printer, unsigned long current_time_us) {
    flockfile(stdout);
    log_time(current_time_us, reference_time_us);
    printf("printer%d is out of paper\n", printer->id);
    funlockfile(stdout);
}

void log_paper_refill_start(Printer* printer, int papers_needed, 
    int time_to_refill_ms, unsigned long current_time_us)
{
    flockfile(stdout);
    log_time(current_time_us, reference_time_us);
    int time_ms = time_to_refill_ms / 1000;
    int time_us = time_to_refill_ms % 1000;
    printf("printer%d starts refilling %d papers, estimated time = %d.%03dms\n",
        printer->id, papers_needed, time_ms, time_us);
    funlockfile(stdout);
}

void log_paper_refill_end(Printer* printer, int refill_duration_ms,
    unsigned long current_time_us)
{
    flockfile(stdout);
    log_time(current_time_us, reference_time_us);
    int time_ms = refill_duration_ms / 1000;
    int time_us = refill_duration_ms % 1000;
    printf("printer%d finishes refilling paper, actual time = %d.%03d ms\n",
        printer->id, time_ms, time_us);
    funlockfile(stdout);
}

void log_ctrl_c_pressed(SimulationStatistics* stats) {
    reference_end_time_us = get_time_in_us();
    flockfile(stdout);
    log_time(reference_end_time_us, reference_time_us);
    stats->simulation_duration_us = reference_end_time_us - reference_time_us;
    printf("simulation stopped, duration = %d.%03dms\n",
        (int)(stats->simulation_duration_us / 1000), (int)(stats->simulation_duration_us % 1000));
    funlockfile(stdout);
}
