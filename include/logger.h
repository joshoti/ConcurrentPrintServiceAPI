#ifndef LOGGER_H
#define LOGGER_H

struct Job;
struct Printer;

/**
 * @brief Logs the simulation parameters at the start of the simulation.
 */
void log_simulation_parameters();

/**
 * @brief Logs the start of the simulation.
 * @return The reference start time in microseconds.
 */
unsigned long log_simulation_start();

/**
 * @brief Logs the end of the simulation.
 * @return The reference end time in microseconds.
 */
unsigned long log_simulation_end();

/**
 * @brief Logs an event when a new job is created in the system.
 *
 * @param job The job that has been created.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_system_arrival(struct Job, unsigned long current_time_us);
/**
 * @brief Logs an event when a job is removed from the system without being processed.
 *
 * @param job The job that has been removed.
 */
void log_removed_job(struct Job);
/**
 * @brief Logs an event when a job is dropped from the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_dropped_job(struct Job, unsigned long previous_job_arrival_time_us,
    unsigned long current_time_us);

/**
 * @brief Logs an event when a job arrives at the queue.
 * @param job The job that has arrived at the queue.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_queue_arrival(struct Job, unsigned long current_time_us);
/**
 * @brief Logs an event when a job departs from the queue.
 *
 * @param job The job that has departed from the queue.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_queue_departure(struct Job, unsigned long current_time_us);

/**
 * @brief Logs an event when a job arrives at a printer for processing.
 *
 * @param job The job that has arrived at the printer.
 * @param printer The printer that the job has arrived at.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_printer_arrival(struct Job, struct Printer,
    unsigned long current_time_us);
/**
 * @brief Logs an event when a job departs from a printer after processing.
 * @param job The job that has departed from the printer.
 * @param printer The printer that the job has departed from.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_system_departure(struct Job, struct Printer,
    unsigned long current_time_us);

/**
 * @brief Logs an event when a printer runs out of paper.
 *
 * @param printer The printer that has run out of paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_empty(struct Printer, unsigned long current_time_us);
/**
 * @brief Logs an event when a printer starts refilling paper.
 *
 * @param printer The printer that is starting to refill paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_refill_start(struct Printer, unsigned long current_time_us);
/**
 * @brief Logs an event when a printer finishes refilling paper.
 *
 * @param printer The printer that has finished refilling paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_refill_end(struct Printer, unsigned long current_time_us);

/**
 * @brief Logs an event when Ctrl+C is pressed to terminate the simulation.
 */
void log_ctrl_c_pressed();

#endif // LOGGER_H