#ifndef EVENT_PUBLISHER_H
#define EVENT_PUBLISHER_H

struct Job;
struct Printer;
struct SimulationParameters;
struct mg_connection;
struct SimulationStatistics;
struct TimedQueue;

/**
 * @brief Publishes the simulation parameters.
 * 
 * @param params The simulation parameters to publish.
 * @param ws_conn The WebSocket connection to publish the parameters to.
 */
void publish_simulation_parameters(const SimulationParameters* params, struct mg_connection* ws_conn);
/**
 * @brief Publishes the start of the simulation.
 * 
 * @param stats The simulation statistics to update.
 * @param ws_conn The WebSocket connection to publish the start event to.
 */
void publish_simulation_start(SimulationStatistics* stats, struct mg_connection* ws_conn);
/**
 * @brief Publishes the end of the simulation.
 * 
 * @param stats The simulation statistics to update.
 * @param ws_conn The WebSocket connection to publish the end event to.
 */
void publish_simulation_end(SimulationStatistics* stats, struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when a new job is created in the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_system_arrival(Job* job,
    unsigned long previous_job_arrival_time_us, SimulationStatistics* stats,
    struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a job is dropped from the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_dropped_job(Job* job,
    unsigned long previous_job_arrival_time_us, SimulationStatistics* stats,
    struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a job is removed from the system without being processed.
 *
 * @param job The job that has been removed.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_removed_job(Job* job, struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when a job arrives at the queue.
 * @param job The job that has arrived at the queue.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_queue_arrival(const struct Job* job, struct SimulationStatistics* stats,
    struct TimedQueue* job_queue, struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a job departs from the queue.
 *
 * @param job The job that has departed from the queue.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_queue_departure(const struct Job* job, struct SimulationStatistics* stats,
    struct TimedQueue* job_queue, struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when a job arrives at a printer for processing.
 *
 * @param job The job that has arrived at the printer.
 * @param printer The printer that the job has arrived at.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_printer_arrival(const Job* job, const Printer* printer,
    struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a job departs from a printer after processing.
 * @param job The job that has departed from the printer.
 * @param printer The printer that the job has departed from.
 * @param stats The simulation statistics to update.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_system_departure(const Job* job, const Printer* printer,
    SimulationStatistics* stats, struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when a printer runs out of paper.
 *
 * @param printer The printer that has run out of paper.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_paper_empty(Printer* printer, unsigned long current_time_us,
    struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a printer starts refilling paper.
 *
 * @param printer The printer that is starting to refill paper.
 * @param papers_needed The number of papers needed to refill the printer to full capacity.
 * @param time_to_refill_ms The time in milliseconds it will take to refill the paper
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_paper_refill_start(Printer* printer, int papers_needed,
    int time_to_refill_ms, unsigned long current_time_us, struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a printer finishes refilling paper.
 *
 * @param printer The printer that has finished refilling paper.
 * @param refill_duration_ms The time in milliseconds it took to refill the paper.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_paper_refill_end(Printer* printer, int refill_duration_ms,
    unsigned long current_time_us, struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when the simulation is stopped.
 * 
 * @param stats The simulation statistics to update.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_simulation_stopped(SimulationStatistics* stats, struct mg_connection* ws_conn);

/**
 * @brief Calculates and publishes all relevant simulation statistics via WebSocket.
 *
 * @param stats A simulation statistics struct.
 * @param ws_conn The WebSocket connection to publish the statistics to.
 */
void publish_statistics(SimulationStatistics* stats, struct mg_connection* ws_conn);

#endif // EVENT_PUBLISHER_H