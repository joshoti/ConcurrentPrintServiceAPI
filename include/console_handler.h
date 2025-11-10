#ifndef CONSOLE_HANDLER_H
#define CONSOLE_HANDLER_H

struct job;
struct printer;
struct simulation_parameters;
struct simulation_statistics;
struct timed_queue;
/**
 * @brief Logs the simulation parameters at the start of the simulation.
 * 
 * @param params The simulation parameters to log.
 */
void log_simulation_parameters(const struct simulation_parameters* params);

/**
 * @brief Logs the start of the simulation.
 * 
 * @param stats The simulation statistics to update.
 */
void log_simulation_start(struct simulation_statistics* stats);

/**
 * @brief Logs the end of the simulation.
 * 
 * @param stats The simulation statistics to update.
 */
void log_simulation_end(struct simulation_statistics* stats);

/**
 * @brief Logs an event when a new job is created in the system.
 *
 * @param job The job that has been created.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 */
void log_system_arrival(struct job* job, unsigned long previous_job_arrival_time_us,
    struct simulation_statistics* stats);

/**
 * @brief Logs an event when a job is dropped from the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 */
void log_dropped_job(struct job* job, unsigned long previous_job_arrival_time_us,
    struct simulation_statistics* stats);

/**
 * @brief Logs an event when a job is removed from the system without being processed.
 *
 * @param job The job that has been removed.
 */
void log_removed_job(struct job* job);

/**
 * @brief Logs an event when a job arrives at the queue.
 * @param job The job that has arrived at the queue.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 * @param last_interaction_time_us The last interaction time of the queue in microseconds.
 */
void log_queue_arrival(const struct job* job, struct simulation_statistics* stats,
    struct timed_queue* job_queue, unsigned long last_interaction_time_us);
/**
 * @brief Logs an event when a job departs from the queue.
 *
 * @param job The job that has departed from the queue.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 * @param last_interaction_time_us The last interaction time of the queue in microseconds.
 */
void log_queue_departure(const struct job* job, struct simulation_statistics* stats,
    struct timed_queue* job_queue, unsigned long last_interaction_time_us);

/**
 * @brief Logs an event when a job arrives at a printer for processing.
 *
 * @param job The job that has arrived at the printer.
 * @param printer The printer that the job has arrived at.
 */
void log_printer_arrival(const struct job* job, const struct printer* printer);
/**
 * @brief Logs an event when a job departs from a printer after processing.
 * @param job The job that has departed from the printer.
 * @param printer The printer that the job has departed from.
 * @param stats The simulation statistics to update.
 */
void log_system_departure(const struct job* job, const struct printer* printer,
    struct simulation_statistics* stats);

/**
 * @brief Logs an event when a printer runs out of paper.
 *
 * @param printer The printer that has run out of paper.
 * @param job_id The id of the job that cannot be processed due to lack of paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_empty(struct printer* printer, int job_id, unsigned long current_time_us);
/**
 * @brief Logs an event when a printer starts refilling paper.
 *
 * @param printer The printer that is starting to refill paper.
 * @param papers_needed The number of papers needed to refill the printer to full capacity.
 * @param time_to_refill_us The time it will take to refill the printer in microseconds.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_refill_start(struct printer* printer, int papers_needed, int time_to_refill_us, unsigned long current_time_us);
/**
 * @brief Logs an event when a printer finishes refilling paper.
 *
 * @param printer The printer that has finished refilling paper.
 * @param refill_duration_us The duration of the refill in microseconds.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_refill_end(struct printer* printer, int refill_duration_us, unsigned long current_time_us);

/**
 * @brief Logs an event when Ctrl+C is pressed to terminate the simulation.
 * @param stats The simulation statistics to update.
 */
void log_ctrl_c_pressed(struct simulation_statistics* stats);

/**
 * @brief Registers the console handler with the log router
 */
void console_handler_register(void);

#endif // CONSOLE_HANDLER_H