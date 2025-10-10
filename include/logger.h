#ifndef LOGGER_H
#define LOGGER_H

typedef struct Job;
typedef struct Printer;
typedef struct SimulationParameters;
typedef struct SimulationStatistics;
typedef struct LinkedList;

/**
 * @brief Logs the simulation parameters at the start of the simulation.
 * 
 * @param params The simulation parameters to log.
 */
void log_simulation_parameters(const SimulationParameters* params);

/**
 * @brief Logs the start of the simulation.
 */
void log_simulation_start();

/**
 * @brief Logs the end of the simulation.
 * 
 * @param stats The simulation statistics to update.
 */
void log_simulation_end(SimulationStatistics* stats);

/**
 * @brief Logs an event when a new job is created in the system.
 *
 * @param job The job that has been created.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 */
void log_system_arrival(Job* job, unsigned long previous_job_arrival_time_us,
    SimulationStatistics* stats);

/**
 * @brief Logs an event when a job is dropped from the system.
 *
 * @param job The job that has been dropped.
 * @param previous_job_arrival_time_us The arrival time of the previous job in microseconds.
 * @param stats The simulation statistics to update.
 */
void log_dropped_job(Job* job, unsigned long previous_job_arrival_time_us,
    SimulationStatistics* stats);

/**
 * @brief Logs an event when a job is removed from the system without being processed.
 *
 * @param job The job that has been removed.
 */
void log_removed_job(Job* job);

/**
 * @brief Logs an event when a job arrives at the queue.
 * @param job The job that has arrived at the queue.
 * @param last_interaction_time_us A pointer to the queue's last interaction time in microseconds.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 */
void log_queue_arrival(const Job* job, unsigned long* last_interaction_time_us,
    SimulationStatistics* stats, LinkedList* job_queue);
/**
 * @brief Logs an event when a job departs from the queue.
 *
 * @param job The job that has departed from the queue.
 * @param last_interaction_time_us A pointer to the queue's last interaction time in microseconds.
 * @param stats The simulation statistics to update.
 * @param job_queue The job queue to check the length of.
 */
void log_queue_departure(const Job* job, unsigned long* last_interaction_time_us,
    SimulationStatistics* stats, LinkedList* job_queue);

/**
 * @brief Logs an event when a job arrives at a printer for processing.
 *
 * @param job The job that has arrived at the printer.
 * @param printer The printer that the job has arrived at.
 */
void log_printer_arrival(const Job* job, const Printer* printer);
/**
 * @brief Logs an event when a job departs from a printer after processing.
 * @param job The job that has departed from the printer.
 * @param printer The printer that the job has departed from.
 * @param stats The simulation statistics to update.
 */
void log_system_departure(const Job* job, const Printer* printer,
    SimulationStatistics* stats);

/**
 * @brief Logs an event when a printer runs out of paper.
 *
 * @param printer The printer that has run out of paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_empty(Printer* printer, unsigned long current_time_us);
/**
 * @brief Logs an event when a printer starts refilling paper.
 *
 * @param printer The printer that is starting to refill paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_refill_start(Printer* printer, unsigned long current_time_us);
/**
 * @brief Logs an event when a printer finishes refilling paper.
 *
 * @param printer The printer that has finished refilling paper.
 * @param current_time_us The current simulation time in microseconds.
 */
void log_paper_refill_end(Printer* printer, unsigned long current_time_us);

/**
 * @brief Logs an event when Ctrl+C is pressed to terminate the simulation.
 * @param stats The simulation statistics to update.
 */
void log_ctrl_c_pressed(SimulationStatistics* stats);

#endif // LOGGER_H