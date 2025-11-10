#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

struct job;
struct printer;
struct simulation_parameters;
struct simulation_statistics;
struct timed_queue;

/**
 * @brief Publishes the simulation parameters.
 * 
 * @param params The simulation parameters to publish.
 */
void publish_simulation_parameters(const struct simulation_parameters* params);
/**
 * @brief Publishes the start of the simulation.
 * 
 * @param stats The simulation statistics to update.
 */
void publish_simulation_start(struct simulation_statistics* stats);
/**
 * @brief Publishes the end of the simulation.
 * 
 * @param stats The simulation statistics to update.
 */
void publish_simulation_end(struct simulation_statistics* stats);

/**
 * @brief Publishes an event when a new job is created in the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 */
void publish_system_arrival(struct job* job,
    unsigned long previous_job_arrival_time_us, struct simulation_statistics* stats);
/**
 * @brief Publishes an event when a job is dropped from the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 */
void publish_dropped_job(struct job* job,
    unsigned long previous_job_arrival_time_us, struct simulation_statistics* stats);
/**
 * @brief Publishes an event when a job is removed from the system without being processed.
 *
 * @param job The job that has been removed.
 */
void publish_removed_job(struct job* job);

/**
 * @brief Publishes an event when a job arrives at the queue.
 * @param job The job that has arrived at the queue.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 * @param last_interaction_time_us The last interaction time of the job queue.
 */
void publish_queue_arrival(const struct job* job, struct simulation_statistics* stats,
    struct timed_queue* job_queue, unsigned long last_interaction_time_us);
/**
 * @brief Publishes an event when a job departs from the queue.
 *
 * @param job The job that has departed from the queue.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 * @param last_interaction_time_us The last interaction time of the job queue.
 */
void publish_queue_departure(const struct job* job, struct simulation_statistics* stats,
    struct timed_queue* job_queue, unsigned long last_interaction_time_us);

/**
 * @brief Publishes an event when a job arrives at a printer for processing.
 *
 * @param job The job that has arrived at the printer.
 * @param printer The printer that the job has arrived at.
 */
void publish_printer_arrival(const struct job* job, const struct printer* printer);
/**
 * @brief Publishes an event when a job departs from a printer after processing.
 * @param job The job that has departed from the printer.
 * @param printer The printer that the job has departed from.
 * @param stats The simulation statistics to update.
 */
void publish_system_departure(const struct job* job, const struct printer* printer,
    struct simulation_statistics* stats);

/**
 * @brief Publishes an event when a printer runs out of paper.
 *
 * @param printer The printer that has run out of paper.
 * @param job_id The id of the job that cannot be processed due to lack of paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_paper_empty(struct printer* printer, int job_id, unsigned long current_time_us);
/**
 * @brief Publishes an event when a printer starts refilling paper.
 *
 * @param printer The printer that is starting to refill paper.
 * @param papers_needed The number of papers needed to refill the printer to full capacity.
 * @param time_to_refill_us The time in microseconds it will take to refill the paper
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_paper_refill_start(struct printer* printer, int papers_needed,
    int time_to_refill_us, unsigned long current_time_us);
/**
 * @brief Publishes an event when a printer finishes refilling paper.
 *
 * @param printer The printer that has finished refilling paper.
 * @param refill_duration_us The time in microseconds it took to refill the paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_paper_refill_end(struct printer* printer, int refill_duration_us,
    unsigned long current_time_us);

/**
 * @brief Publishes an event when the simulation is stopped.
 * 
 * @param stats The simulation statistics to update.
 */
void publish_simulation_stopped(struct simulation_statistics* stats);

/**
 * @brief Calculates and publishes all relevant simulation statistics via WebSocket.
 *
 * @param stats A simulation statistics struct.
 */
void publish_statistics(struct simulation_statistics* stats);

/**
 * @brief Registers the websocket handler with the log router
 */
void websocket_handler_register(void);

#endif // WEBSOCKET_HANDLER_H