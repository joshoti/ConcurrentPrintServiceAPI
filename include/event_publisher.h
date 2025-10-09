#ifndef EVENT_PUBLISHER_H
#define EVENT_PUBLISHER_H

struct Job;
struct Printer;
struct SimulationParameters;
struct mg_connection;
struct SimulationStatistics;
struct LinkedList;

/**
 * @brief Publishes the simulation parameters.
 * 
 * @param params The simulation parameters to publish.
 * @param ws_conn The WebSocket connection to publish the parameters to.
 */
void publish_simulation_parameters(const struct SimulationParameters* params, struct mg_connection* ws_conn);
/**
 * @brief Publishes the start of the simulation.
 * 
 * @param ws_conn The WebSocket connection to publish the start event to.
 *
 * @return The timestamp (in microseconds) when the simulation started.
 */
unsigned long publish_simulation_start(struct mg_connection* ws_conn);
/**
 * @brief Publishes the end of the simulation.
 * 
 * @param stats The simulation statistics to update.
 * @param ws_conn The WebSocket connection to publish the end event to.
 *
 * @return The timestamp (in microseconds) when the simulation ended.
 */
unsigned long publish_simulation_end(struct SimulationStatistics* stats, struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when a new job is created in the system.
 *
 * @param job The job that has been created.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_system_arrival(struct Job* job, unsigned long current_time_us, struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a job is removed from the system without being processed.
 *
 * @param job The job that has been removed.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_removed_job(struct Job* job, struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a job is dropped from the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_dropped_job(struct Job* job, unsigned long previous_job_arrival_time_us,
    unsigned long current_time_us, struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when a job arrives at the queue.
 * @param job The job that has arrived at the queue.
 * @param current_time_us The current simulation time in microseconds.
 * @param job_queue The job queue to check the length of.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_queue_arrival(const struct Job* job, unsigned long current_time_us,
    struct LinkedList* job_queue, struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a job departs from the queue.
 *
 * @param job The job that has departed from the queue.
 * @param current_time_us The current simulation time in microseconds.
 * @param job_queue The job queue to check the length of.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_queue_departure(const struct Job* job, unsigned long current_time_us,
    struct LinkedList* job_queue, struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when a job arrives at a printer for processing.
 *
 * @param job The job that has arrived at the printer.
 * @param printer The printer that the job has arrived at.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_printer_arrival(const struct Job* job, const struct Printer* printer,
    unsigned long current_time_us, struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a job departs from a printer after processing.
 * @param job The job that has departed from the printer.
 * @param printer The printer that the job has departed from.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_system_departure(const struct Job* job, const struct Printer* printer,
    unsigned long current_time_us, SimulationStatistics* stats,
    struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when a printer runs out of paper.
 *
 * @param printer The printer that has run out of paper.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_paper_empty(struct Printer, unsigned long current_time_us,
    struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a printer starts refilling paper.
 *
 * @param printer The printer that is starting to refill paper.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_paper_refill_start(struct Printer, unsigned long current_time_us,
    struct mg_connection* ws_conn);
/**
 * @brief Publishes an event when a printer finishes refilling paper.
 *
 * @param printer The printer that has finished refilling paper.
 * @param current_time_us The current simulation time in microseconds.
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_paper_refill_end(struct Printer, unsigned long current_time_us,
    struct mg_connection* ws_conn);

/**
 * @brief Publishes an event when the simulation is stopped.
 * 
 * @param ws_conn The WebSocket connection to publish the event to.
 */
void publish_simulation_stopped(struct mg_connection* ws_conn);

#endif // EVENT_PUBLISHER_H