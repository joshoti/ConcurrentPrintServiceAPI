#ifndef LOGGER_H
#define LOGGER_H

struct Job;
struct Printer;
struct SimulationParameters;
struct SimulationStatistics;
struct TimedQueue;

/**
 * @brief Logs the simulation parameters at the start of the simulation.
 * 
 * @param params The simulation parameters to log.
 */
void log_simulation_parameters(const struct SimulationParameters* params);

/**
 * @brief Logs the start of the simulation.
 * 
 * @param stats The simulation statistics to update.
 */
void log_simulation_start(struct SimulationStatistics* stats);

/**
 * @brief Logs the end of the simulation.
 * 
 * @param stats The simulation statistics to update.
 */
void log_simulation_end(struct SimulationStatistics* stats);

/**
 * @brief Logs an event when a new job is created in the system.
 *
 * @param job The job that has been created.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 */
void log_system_arrival(struct Job* job, unsigned long previous_job_arrival_time_us,
    struct SimulationStatistics* stats);

/**
 * @brief Logs an event when a job is dropped from the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 */
void log_dropped_job(struct Job* job, unsigned long previous_job_arrival_time_us,
    struct SimulationStatistics* stats);

/**
 * @brief Logs an event when a job is removed from the system without being processed.
 *
 * @param job The job that has been removed.
 */
void log_removed_job(struct Job* job);

/**
 * @brief Logs an event when a job arrives at the queue.
 * @param job The job that has arrived at the queue.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 */
void log_queue_arrival(const struct Job* job, struct SimulationStatistics* stats,
    struct TimedQueue* job_queue);
/**
 * @brief Logs an event when a job departs from the queue.
 *
 * @param job The job that has departed from the queue.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 */
void log_queue_departure(const struct Job* job, struct SimulationStatistics* stats,
    struct TimedQueue* job_queue);

/**
 * @brief Logs an event when a job arrives at a printer for processing.
 *
 * @param job The job that has arrived at the printer.
 * @param printer The printer that the job has arrived at.
 */
void log_printer_arrival(const struct Job* job, const struct Printer* printer);
/**
 * @brief Logs an event when a job departs from a printer after processing.
 * @param job The job that has departed from the printer.
 * @param printer The printer that the job has departed from.
 * @param stats The simulation statistics to update.
 */
void log_system_departure(const struct Job* job, const struct Printer* printer,
    struct SimulationStatistics* stats);

/**
 * @brief Logs an event when a printer runs out of paper.
 *
 * @param printer The printer that has run out of paper.
 * @param job_id The id of the job that cannot be processed due to lack of paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_empty(struct Printer* printer, int job_id, unsigned long current_time_us);
/**
 * @brief Logs an event when a printer starts refilling paper.
 *
 * @param printer The printer that is starting to refill paper.
 * @param papers_needed The number of papers needed to refill the printer to full capacity.
 * @param time_to_refill_ms The time it will take to refill the printer in milliseconds.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_refill_start(struct Printer* printer, int papers_needed, int time_to_refill_ms, unsigned long current_time_us);
/**
 * @brief Logs an event when a printer finishes refilling paper.
 *
 * @param printer The printer that has finished refilling paper.
 * @param refill_duration_ms The duration of the refill in milliseconds.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_refill_end(struct Printer* printer, int refill_duration_ms, unsigned long current_time_us);

/**
 * @brief Logs an event when Ctrl+C is pressed to terminate the simulation.
 * @param stats The simulation statistics to update.
 */
void log_ctrl_c_pressed(struct SimulationStatistics* stats);

#endif // LOGGER_H