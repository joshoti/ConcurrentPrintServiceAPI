#include <stdio.h>
#include <string.h>
#include "common.h"
#include "event_publisher.h"
#include "preprocessing.h"
#include "timeutils.h"
#include "civetweb.h"
#include "simulation_stats.h"
#include "job_receiver.h"
#include "linked_list.h"
#include "printer.h"

static unsigned long reference_time_us = 0;
static unsigned long reference_end_time_us = 0;

/**
 * @brief Writes to buffer the given time in milliseconds (ms) relative to the given reference
 * time.
 * 
 * @param time_us The time to log, in microseconds (us).
 * @param reference_time_us The reference time to log against, in microseconds (us).
 * @param buf The buffer to write the formatted time string into.
 *
 * Sample output called 250ms (250,000us) after reference time is "00000251.457ms: "
 */
static void write_time_to_buffer(unsigned long time_us, unsigned long reference_time_us, char* buf) {
    time_us -= reference_time_us;

    int milliseconds = 0;
    int microseconds = 0;
    time_in_us_to_ms(time_us, &milliseconds, &microseconds);
    sprintf(buf, time_format, milliseconds, microseconds);
}

void publish_simulation_parameters(const SimulationParameters* params, struct mg_connection* ws_conn) {
    char buf[1024];
    sprintf(buf, "{\"type\":\"params\", \"params\": {\"job_arrival_time\":%.6g,\
        \"printing_rate\":%.6g, \"queue_capacity\":%d,\
        \"printer_paper_capacity\":%d, \"refill_rate\":%.6g, \"num_jobs\":%d,\
        \"papers_required_lower_bound\":%d, \"papers_required_upper_bound\":%d}}",
            params->job_arrival_time_us, params->printing_rate, params->queue_capacity,
            params->printer_paper_capacity, params->refill_rate, params->num_jobs,
            params->papers_required_lower_bound, params->papers_required_upper_bound);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_simulation_start(SimulationStatistics* stats, struct mg_connection* ws_conn) {
    reference_time_us = get_time_in_us();
    stats->simulation_start_time_us = reference_time_us;
    char time_buf[64];
    char buf[1024];

    write_time_to_buffer(reference_time_us, reference_time_us, time_buf);
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s simulation begins\"}", time_buf);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
    return reference_time_us;
}

void publish_simulation_end(SimulationStatistics* stats,
    struct mg_connection* ws_conn)
{
    reference_end_time_us = get_time_in_us();
    char time_buf[64];
    char buf[1024];

    write_time_to_buffer(reference_end_time_us, reference_time_us, time_buf);
    stats->simulation_duration_us = reference_end_time_us - reference_time_us;
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s simulation ends, duration = %d.%03dms\"}",
        time_buf, stats->simulation_duration_us / 1000, stats->simulation_duration_us % 1000);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}


/**
 * @brief Publishes an event when a new job is created in the system or when a job is dropped
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
    int is_dropped, SimulationStatistics* stats, struct mg_connection* ws_conn)
{
    char time_buf[64];
    char buf[1024];

    write_time_to_buffer(current_job_arrival_time_us, reference_time_us, time_buf);

    int inter_arrival_time_us = current_job_arrival_time_us - previous_job_arrival_time_us;
    stats->total_inter_arrival_time_us += inter_arrival_time_us; // stats: avg job inter-arrival time
    stats->total_jobs_arrived += 1; // stats: total jobs arrived
    int time_in_ms = inter_arrival_time_us / 1000;
    int time_in_us = inter_arrival_time_us % 1000;
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s job%d arrives, needs %d paper%s, inter-arrival time = %d.%03dms%s\"}",
        time_buf, job_id, papers_required,
        papers_required == 1 ? "" : "s", time_in_ms, time_in_us,
        is_dropped ? ", dropped" : "");
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_system_arrival(Job* job, unsigned long previous_job_arrival_time_us,
    SimulationStatistics* stats, struct mg_connection* ws_conn)
{
    job_arrival_helper(job->id, job->papers_required,
        previous_job_arrival_time_us, job->system_arrival_time_us, FALSE,
        stats, ws_conn);
}

void publish_dropped_job(Job* job, unsigned long previous_job_arrival_time_us,
    SimulationStatistics* stats, struct mg_connection* ws_conn)
{
    stats->total_jobs_dropped += 1; // stats: total jobs dropped
    job_arrival_helper(job->id, job->papers_required,
        previous_job_arrival_time_us, job->system_arrival_time_us, TRUE,
        stats, ws_conn);
}

void publish_removed_job(Job* job, struct mg_connection* ws_conn)
{
    unsigned long current_time_us = get_time_in_us();
    char time_buf[64];
    char buf[1024];

    write_time_to_buffer(current_time_us, reference_time_us, time_buf);
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s job%d removed from system\"}",
        time_buf, job->id);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

// TODO: (DONE) Maintain last interaction time for queue length stats
//      This should be updated on both arrival and departure
//      of jobs to/from the queue
// TODO: Before calling this, update max_job_queue_length if needed
//      or pass stats as a parameter to update it here
void publish_queue_arrival(const Job* job,
    unsigned long* last_interaction_time_us, SimulationStatistics* stats,
    LinkedList* job_queue, struct mg_connection* ws_conn)
{
    stats->area_num_in_job_queue_us +=
        (job->queue_arrival_time_us - *last_interaction_time_us) * // duration of previous state
        (list_length(job_queue) - 1); // -1 for the job that just entered the queue
    // stats: avg job queue length
    *last_interaction_time_us = job->queue_arrival_time_us;

    char time_buf[64];
    char buf[1024];
    write_time_to_buffer(job->queue_arrival_time_us, reference_time_us, time_buf);

    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s job%d enters queue, queue length = %d\"}",
        time_buf, job->id, list_length(job_queue));
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_queue_departure(const Job* job,
    unsigned long* last_interaction_time_us, SimulationStatistics* stats,
    LinkedList* job_queue, struct mg_connection* ws_conn)
{
    stats->area_num_in_job_queue_us +=
        (job->queue_departure_time_us - *last_interaction_time_us) * // duration of previous state
        (list_length(job_queue) + 1); // +1 for the job that just left the queue
    // stats: avg job queue length
    *last_interaction_time_us = job->queue_departure_time_us;

    char time_buf[64];
    char buf[1024];
    write_time_to_buffer(job->queue_departure_time_us, reference_time_us, time_buf);

    int queue_duration = job->queue_departure_time_us - job->queue_arrival_time_us;
    int time_ms = queue_duration / 1000;
    int time_us = queue_duration % 1000;
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s job%d leaves queue, time in queue = %d.%03dms, queue_length = %d\"}",
        time_buf, job->id, time_ms, time_us, list_length(job_queue));
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_printer_arrival(const Job* job, const Printer* printer,
    struct mg_connection* ws_conn)
{
    char time_buf[64];
    char buf[1024];
    write_time_to_buffer(job->service_arrival_time_us, reference_time_us, time_buf);
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s job%d begins service at printer%d, printing %d pages in about %dms\"}",
        time_buf, job->id, printer->id, job->papers_required, job->service_time_requested_ms);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_system_departure(const Job* job, const Printer* printer,
    SimulationStatistics* stats, struct mg_connection* ws_conn)
{
    char time_buf[64];
    char buf[1024];
    write_time_to_buffer(job->service_departure_time_us, reference_time_us, time_buf);

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
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s job%d departs from printer%d, service time = %d.%03dms\"}",
        time_buf, job->id, printer->id, time_ms, time_us);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_paper_empty(Printer* printer, unsigned long current_time_us,
    struct mg_connection* ws_conn)
{
    char time_buf[64];
    char buf[1024];
    write_time_to_buffer(current_time_us, reference_time_us, time_buf);
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s printer%d is out of paper\"}",
        time_buf, printer->id);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_paper_refill_start(Printer* printer, int papers_needed,
    int time_to_refill_ms, unsigned long current_time_us, struct mg_connection* ws_conn)
{
    char time_buf[64];
    char buf[1024];
    write_time_to_buffer(current_time_us, reference_time_us, time_buf);
    int time_ms = time_to_refill_ms / 1000;
    int time_us = time_to_refill_ms % 1000;
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s printer%d starts refilling %d papers, estimated time = %d.%03dms\"}",
        time_buf, printer->id, papers_needed, time_ms, time_us);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_paper_refill_end(Printer* printer, int refill_duration_ms,
    unsigned long current_time_us, struct mg_connection* ws_conn)
{
    char time_buf[64];
    char buf[1024];
    write_time_to_buffer(current_time_us, reference_time_us, time_buf);
    int time_ms = refill_duration_ms / 1000;
    int time_us = refill_duration_ms % 1000;
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s printer%d finishes refilling, actual time = %d.%03dms\"}",
        time_buf, printer->id, time_ms, time_us);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_simulation_stopped(SimulationStatistics* stats, struct mg_connection* ws_conn) {
    char time_buf[64];
    char buf[1024];
    reference_end_time_us = get_time_in_us();

    write_time_to_buffer(reference_end_time_us, reference_time_us, time_buf);
    stats->simulation_duration_us = reference_end_time_us - reference_time_us;
    sprintf(buf, "{\"type\":\"log\", \"message\":\"%s simulation stopped, duration = %d.%03dms\"}",
        time_buf, stats->simulation_duration_us / 1000, stats->simulation_duration_us % 1000);
    if (ws_conn) {
        mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
    }
}

void publish_statistics(SimulationStatistics* stats, struct mg_connection* ws_conn) {
    if (stats == NULL) return;

    char buf[4096];
    if (write_statistics_to_buffer(stats, buf, sizeof(buf)) > 0) {
        if (ws_conn) {
            mg_websocket_write(ws_conn, MG_WEBSOCKET_OPCODE_TEXT, buf, strlen(buf));
        }
    }
}
