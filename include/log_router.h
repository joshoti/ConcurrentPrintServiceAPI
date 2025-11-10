#ifndef LOG_ROUTER_H
#define LOG_ROUTER_H

#include <stddef.h>

// Forward decls to avoid pulling in all headers here
struct job;
struct printer;
struct simulation_parameters;
struct simulation_statistics;
struct timed_queue;

// Output modes
#define LOG_MODE_TERMINAL 0
#define LOG_MODE_SERVER   1

extern int g_log_mode;

void set_log_mode(int mode);

// Publisher backend (set by server at startup). Keeping this in the router header
// avoids a hard link-time dependency on the publisher in CLI mode.
struct PublisherBackend {
    void (*publish_simulation_parameters)(const struct simulation_parameters* params);
    void (*publish_simulation_start)(struct simulation_statistics* stats);
    void (*publish_simulation_end)(struct simulation_statistics* stats);

    void (*publish_system_arrival)(struct job* job, unsigned long previous_job_arrival_time_us,
                                   struct simulation_statistics* stats);
    void (*publish_dropped_job)(struct job* job, unsigned long previous_job_arrival_time_us,
                                struct simulation_statistics* stats);
    void (*publish_removed_job)(struct job* job);

    void (*publish_queue_arrival)(const struct job* job, struct simulation_statistics* stats,
                                  struct timed_queue* job_queue, unsigned long last_interaction_time_us);
    void (*publish_queue_departure)(const struct job* job, struct simulation_statistics* stats,
                                    struct timed_queue* job_queue, unsigned long last_interaction_time_us);

    void (*publish_printer_arrival)(const struct job* job, const struct printer* printer);
    void (*publish_system_departure)(const struct job* job, const struct printer* printer,
                                     struct simulation_statistics* stats);

    void (*publish_paper_empty)(struct printer* printer, int job_id, unsigned long current_time_us);
    void (*publish_paper_refill_start)(struct printer* printer, int papers_needed,
                                       int time_to_refill_us, unsigned long current_time_us);
    void (*publish_paper_refill_end)(struct printer* printer, int refill_duration_us,
                                     unsigned long current_time_us);

    void (*publish_simulation_stopped)(struct simulation_statistics* stats);
    void (*publish_statistics)(struct simulation_statistics* stats);
};

void log_router_set_publisher_backend(const struct PublisherBackend* pubs);

// Logger backend (set by CLI at startup). This removes a hard dependency
// on logger.c from the server build.
struct LoggerBackend {
    void (*log_simulation_parameters)(const struct simulation_parameters* params);
    void (*log_simulation_start)(struct simulation_statistics* stats);
    void (*log_simulation_end)(struct simulation_statistics* stats);

    void (*log_system_arrival)(struct job* job, unsigned long previous_job_arrival_time_us,
                               struct simulation_statistics* stats);
    void (*log_dropped_job)(struct job* job, unsigned long previous_job_arrival_time_us,
                            struct simulation_statistics* stats);
    void (*log_removed_job)(struct job* job);

    void (*log_queue_arrival)(const struct job* job, struct simulation_statistics* stats,
                              struct timed_queue* job_queue, unsigned long last_interaction_time_us);
    void (*log_queue_departure)(const struct job* job, struct simulation_statistics* stats,
                                struct timed_queue* job_queue, unsigned long last_interaction_time_us);

    void (*log_printer_arrival)(const struct job* job, const struct printer* printer);
    void (*log_system_departure)(const struct job* job, const struct printer* printer,
                                 struct simulation_statistics* stats);

    void (*log_paper_empty)(struct printer* printer, int job_id, unsigned long current_time_us);
    void (*log_paper_refill_start)(struct printer* printer, int papers_needed,
                                   int time_to_refill_us, unsigned long current_time_us);
    void (*log_paper_refill_end)(struct printer* printer, int refill_duration_us,
                                 unsigned long current_time_us);

    void (*log_ctrl_c_pressed)(struct simulation_statistics* stats);
    void (*log_statistics)(struct simulation_statistics* stats);
};

void log_router_set_logger_backend(const struct LoggerBackend* logs);

// Wrapper API that routes to stdout logger or websocket publisher
void emit_simulation_parameters(const struct simulation_parameters* params);
void emit_simulation_start(struct simulation_statistics* stats);
void emit_simulation_end(struct simulation_statistics* stats);
void emit_system_arrival(struct job* job, unsigned long previous_job_arrival_time_us,
                         struct simulation_statistics* stats);
void emit_dropped_job(struct job* job, unsigned long previous_job_arrival_time_us,
                      struct simulation_statistics* stats);
void emit_removed_job(struct job* job);
void emit_queue_arrival(const struct job* job, struct simulation_statistics* stats,
                        struct timed_queue* job_queue, unsigned long last_interaction_time_us);
void emit_queue_departure(const struct job* job, struct simulation_statistics* stats,
                          struct timed_queue* job_queue, unsigned long last_interaction_time_us);
void emit_printer_arrival(const struct job* job, const struct printer* printer);
void emit_system_departure(const struct job* job, const struct printer* printer,
                           struct simulation_statistics* stats);
void emit_paper_empty(struct printer* printer, int job_id, unsigned long current_time_us);
void emit_paper_refill_start(struct printer* printer, int papers_needed,
                             int time_to_refill_us, unsigned long current_time_us);
void emit_paper_refill_end(struct printer* printer, int refill_duration_us,
                           unsigned long current_time_us);
void emit_simulation_stopped(struct simulation_statistics* stats);
void emit_statistics(struct simulation_statistics* stats);

#endif // LOG_ROUTER_H
