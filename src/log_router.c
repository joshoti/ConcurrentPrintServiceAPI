#include "log_router.h"

int g_log_mode = LOG_MODE_TERMINAL;

void set_log_mode(int mode) { g_log_mode = mode; }

// Store publisher backend set by the server at startup.
static struct PublisherBackend g_publisher = (struct PublisherBackend){0};
// Store logger backend set by the CLI at startup.
static struct LoggerBackend g_logger = (struct LoggerBackend){0};

void log_router_set_publisher_backend(const struct PublisherBackend* pubs) {
    if (pubs) g_publisher = *pubs;
}

void log_router_set_logger_backend(const struct LoggerBackend* logs) {
    if (logs) g_logger = *logs;
}

void emit_simulation_parameters(const struct SimulationParameters* params) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_simulation_parameters) {
        g_publisher.publish_simulation_parameters(params);
    } else if (g_logger.log_simulation_parameters) {
        g_logger.log_simulation_parameters(params);
    }
}

void emit_simulation_start(struct SimulationStatistics* stats) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_simulation_start) {
        g_publisher.publish_simulation_start(stats);
    } else if (g_logger.log_simulation_start) {
        g_logger.log_simulation_start(stats);
    }
}

void emit_simulation_end(struct SimulationStatistics* stats) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_simulation_end) {
        g_publisher.publish_simulation_end(stats);
    } else if (g_logger.log_simulation_end) {
        g_logger.log_simulation_end(stats);
    }
}

void emit_system_arrival(struct Job* job, unsigned long previous_job_arrival_time_us,
                         struct SimulationStatistics* stats) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_system_arrival) {
        g_publisher.publish_system_arrival(job, previous_job_arrival_time_us, stats);
    } else if (g_logger.log_system_arrival) {
        g_logger.log_system_arrival(job, previous_job_arrival_time_us, stats);
    }
}

void emit_dropped_job(struct Job* job, unsigned long previous_job_arrival_time_us,
                      struct SimulationStatistics* stats) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_dropped_job) {
        g_publisher.publish_dropped_job(job, previous_job_arrival_time_us, stats);
    } else if (g_logger.log_dropped_job) {
        g_logger.log_dropped_job(job, previous_job_arrival_time_us, stats);
    }
}

void emit_removed_job(struct Job* job) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_removed_job) {
        g_publisher.publish_removed_job(job);
    } else if (g_logger.log_removed_job) {
        g_logger.log_removed_job(job);
    }
}

void emit_queue_arrival(const struct Job* job, struct SimulationStatistics* stats,
                        struct TimedQueue* job_queue, unsigned long last_interaction_time_us) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_queue_arrival) {
        g_publisher.publish_queue_arrival(job, stats, job_queue, last_interaction_time_us);
    } else if (g_logger.log_queue_arrival) {
        g_logger.log_queue_arrival(job, stats, job_queue, last_interaction_time_us);
    }
}

void emit_queue_departure(const struct Job* job, struct SimulationStatistics* stats,
                          struct TimedQueue* job_queue, unsigned long last_interaction_time_us) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_queue_departure) {
        g_publisher.publish_queue_departure(job, stats, job_queue, last_interaction_time_us);
    } else if (g_logger.log_queue_departure) {
        g_logger.log_queue_departure(job, stats, job_queue, last_interaction_time_us);
    }
}

void emit_printer_arrival(const struct Job* job, const struct Printer* printer) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_printer_arrival) {
        g_publisher.publish_printer_arrival(job, printer);
    } else if (g_logger.log_printer_arrival) {
        g_logger.log_printer_arrival(job, printer);
    }
}

void emit_system_departure(const struct Job* job, const struct Printer* printer,
                           struct SimulationStatistics* stats) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_system_departure) {
        g_publisher.publish_system_departure(job, printer, stats);
    } else if (g_logger.log_system_departure) {
        g_logger.log_system_departure(job, printer, stats);
    }
}

void emit_paper_empty(struct Printer* printer, int job_id, unsigned long current_time_us) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_paper_empty) {
        g_publisher.publish_paper_empty(printer, job_id, current_time_us);
    } else if (g_logger.log_paper_empty) {
        g_logger.log_paper_empty(printer, job_id, current_time_us);
    }
}

void emit_paper_refill_start(struct Printer* printer, int papers_needed,
                             int time_to_refill_us, unsigned long current_time_us) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_paper_refill_start) {
        g_publisher.publish_paper_refill_start(printer, papers_needed, time_to_refill_us, current_time_us);
    } else if (g_logger.log_paper_refill_start) {
        g_logger.log_paper_refill_start(printer, papers_needed, time_to_refill_us, current_time_us);
    }
}

void emit_paper_refill_end(struct Printer* printer, int refill_duration_us,
                           unsigned long current_time_us) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_paper_refill_end) {
        g_publisher.publish_paper_refill_end(printer, refill_duration_us, current_time_us);
    } else if (g_logger.log_paper_refill_end) {
        g_logger.log_paper_refill_end(printer, refill_duration_us, current_time_us);
    }
}

void emit_ctrl_c_pressed(struct SimulationStatistics* stats) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_simulation_stopped) {
        g_publisher.publish_simulation_stopped(stats);
    } else if (g_logger.log_ctrl_c_pressed) {
        g_logger.log_ctrl_c_pressed(stats);
    }
}

void emit_statistics(struct SimulationStatistics* stats) {
    if (g_log_mode == LOG_MODE_SERVER && g_publisher.publish_statistics) {
        g_publisher.publish_statistics(stats);
    } else if (g_logger.log_statistics) {
        g_logger.log_statistics(stats);
    }
}
