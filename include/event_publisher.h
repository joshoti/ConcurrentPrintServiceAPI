#ifndef EVENT_PUBLISHER_H
#define EVENT_PUBLISHER_H

struct Job;
struct Printer;

/**
 * @brief Publishes the start of the simulation.
 *
 * @return The timestamp (in microseconds) when the simulation started.
 */
unsigned long publish_simulation_start();
/**
 * @brief Publishes the end of the simulation.
 *
 * @return The timestamp (in microseconds) when the simulation ended.
 */
unsigned long publish_simulation_end();

/**
 * @brief Publishes an event when a new job is created in the system.
 *
 * @param job The job that has been created.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_system_arrival(struct Job, unsigned long current_time_us);
/**
 * @brief Publishes an event when a job is removed from the system without being processed.
 *
 * @param job The job that has been removed.
 */
void publish_removed_job(struct Job);
/**
 * @brief Publishes an event when a job is dropped from the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_dropped_job(struct Job, unsigned long previous_job_arrival_time_us,
    unsigned long current_time_us);

/**
 * @brief Publishes an event when a job arrives at the queue.
 * @param job The job that has arrived at the queue.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_queue_arrival(struct Job, unsigned long current_time_us);
/**
 * @brief Publishes an event when a job departs from the queue.
 *
 * @param job The job that has departed from the queue.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_queue_departure(struct Job, unsigned long current_time_us);

/**
 * @brief Publishes an event when a job arrives at a printer for processing.
 *
 * @param job The job that has arrived at the printer.
 * @param printer The printer that the job has arrived at.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_printer_arrival(struct Job, struct Printer,
    unsigned long current_time_us);
/**
 * @brief Publishes an event when a job departs from a printer after processing.
 * @param job The job that has departed from the printer.
 * @param printer The printer that the job has departed from.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_system_departure(struct Job, struct Printer,
    unsigned long current_time_us);

/**
 * @brief Publishes an event when a printer runs out of paper.
 *
 * @param printer The printer that has run out of paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_paper_empty(struct Printer, unsigned long current_time_us);
/**
 * @brief Publishes an event when a printer starts refilling paper.
 *
 * @param printer The printer that is starting to refill paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_paper_refill_start(struct Printer, unsigned long current_time_us);
/**
 * @brief Publishes an event when a printer finishes refilling paper.
 *
 * @param printer The printer that has finished refilling paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void publish_paper_refill_end(struct Printer, unsigned long current_time_us);

/**
 * @brief Publishes an event when the simulation is stopped.
 */
void publish_simulation_stopped();

#endif // EVENT_PUBLISHER_H