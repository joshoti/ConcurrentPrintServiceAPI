#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "simulation_stats.h"
#include "civetweb.h"

// --- Private Helper Functions ---
/**
 * @brief Calculates the average inter-arrival time in seconds.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Average inter-arrival time in seconds.
 */
static double calculate_average_inter_arrival_time(SimulationStatistics* stats) {
    if (stats->total_jobs_arrived <= 1) {
        return 0.0;
    }
    return ((double)stats->total_inter_arrival_time_us / 1000000.0) / (stats->total_jobs_arrived - 1);
}

/**
 * @brief Calculates the average system time in seconds.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Average system time in seconds.
 */
static double calculate_average_system_time(SimulationStatistics* stats) {
    if (stats->total_jobs_served == 0) {
        return 0.0;
    }
    return ((double)stats->total_system_time_us / 1000000.0) / stats->total_jobs_served;
}

/**
 * @brief Calculates the average queue wait time in seconds.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Average queue wait time in seconds.
 */
static double calculate_average_queue_wait_time(SimulationStatistics* stats) {
    if (stats->total_jobs_served == 0) {
        return 0.0;
    }
    return ((double)stats->total_queue_wait_time_us / 1000000.0) / stats->total_jobs_served;
}

/**
 * @brief Calculates the average service time for Printer 1 in seconds.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Average service time for Printer 1 in seconds.
 */
static double calculate_average_service_time_p1(SimulationStatistics* stats) {
    if (stats->jobs_served_by_printer1 == 0) {
        return 0.0;
    }
    return ((double)stats->total_service_time_p1_us / 1000000.0) / stats->jobs_served_by_printer1;
}

/**
 * @brief Calculates the average service time for Printer 2 in seconds.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Average service time for Printer 2 in seconds.
 */
static double calculate_average_service_time_p2(SimulationStatistics* stats) {
    if (stats->jobs_served_by_printer2 == 0) {
        return 0.0;
    }
    return ((double)stats->total_service_time_p2_us / 1000000.0) / stats->jobs_served_by_printer2;
}

/**
 * @brief Calculates the average number of jobs in the queue.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Average number of jobs in the queue.
 */
static double calculate_average_queue_length(SimulationStatistics* stats) {
    if (stats->simulation_duration_us == 0) {
        return 0.0;
    }
    return ((double)stats->area_num_in_job_queue_us) / stats->simulation_duration_us;
}

/**
 * @brief Calculates the standard deviation of system time in seconds.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Standard deviation of system time in seconds.
 */
static double calculate_system_time_std_dev(SimulationStatistics* stats) {
    if (stats->total_jobs_served <= 1) {
        return 0.0;
    }
    double avg_system_time = calculate_average_system_time(stats);
    double avg_system_time_us = avg_system_time * 1000000.0;

    double avg_time_sq = stats->sum_of_system_time_squared_us2 / stats->total_jobs_served;

    double variance = avg_time_sq - (avg_system_time_us * avg_system_time_us);
    return sqrt(variance > 0 ? variance : 0) / 1000000.0; // Convert back to seconds for display consistency
}

/**
 * @brief Calculates the system utilization for Printer 1.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Utilization of Printer 1 (a value between 0 and 1).
 */
static double calculate_system_utilization_p1(SimulationStatistics* stats) {
    if (stats->simulation_duration_us == 0) {
        return 0.0;
    }
    return ((double)stats->total_service_time_p1_us) / stats->simulation_duration_us;
}

/**
 * @brief Calculates the system utilization for Printer 2.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Utilization of Printer 2 (a value between 0 and 1).
 */
static double calculate_system_utilization_p2(SimulationStatistics* stats) {
    if (stats->simulation_duration_us == 0) {
        return 0.0;
    }
    return ((double)stats->total_service_time_p2_us) / stats->simulation_duration_us;
}

/**
 * @brief Calculates the job arrival rate (jobs per second).
 * @param stats Pointer to SimulationStatistics struct.
 * @return Job arrival rate in jobs per second.
 */
static double calculate_job_arrival_rate(SimulationStatistics* stats) {
    if (stats->simulation_duration_us == 0) {
        return 0.0;
    }
    // Convert simulation duration from microseconds to seconds (multiply by 1.0e-6)
    double simulation_duration_sec = stats->simulation_duration_us * 1.0e-6;
    return stats->total_jobs_arrived / simulation_duration_sec;
}

/**
 * @brief Calculates the job drop probability.
 * @param stats Pointer to SimulationStatistics struct.
 * @return Job drop probability (a value between 0 and 1).
 */
static double calculate_job_drop_probability(SimulationStatistics* stats) {
    if (stats->total_jobs_arrived == 0) {
        return 0.0;
    }
    return stats->total_jobs_dropped / stats->total_jobs_arrived;
}

// --- Public API Function Implementations ---
int write_statistics_to_buffer(SimulationStatistics* stats, char* buf, int buf_size) {
    if (stats == NULL || buf == NULL || buf_size <= 0) return -1;

    // Calculate derived statistics
    double avg_inter_arrival_time = calculate_average_inter_arrival_time(stats);
    double avg_system_time = calculate_average_system_time(stats);
    double avg_queue_wait_time = calculate_average_queue_wait_time(stats);
    double avg_service_time_p1 = calculate_average_service_time_p1(stats);
    double avg_service_time_p2 = calculate_average_service_time_p2(stats);
    double avg_queue_length = calculate_average_queue_length(stats);
    double system_time_std_dev = calculate_system_time_std_dev(stats);
    double utilization_p1 = calculate_system_utilization_p1(stats);
    double utilization_p2 = calculate_system_utilization_p2(stats);
    double job_arrival_rate = calculate_job_arrival_rate(stats);
    double job_drop_probability = calculate_job_drop_probability(stats);
    double simulation_time_sec = stats->simulation_duration_us / 1000000.0;
    
    // Build comprehensive JSON statistics message
    int len = snprintf(buf, buf_size,
        "{\"type\":\"statistics\", \"data\":{"
        "\"simulation_duration_sec\":%.6g,"
        "\"total_jobs_arrived\":%.0f,"
        "\"total_jobs_served\":%.0f,"
        "\"total_jobs_dropped\":%.0f,"
        "\"total_jobs_removed\":%.0f,"
        "\"job_arrival_rate_per_sec\":%.6g,"
        "\"job_drop_probability\":%.6g,"
        "\"avg_inter_arrival_time_sec\":%.6g,"
        "\"avg_system_time_sec\":%.6g,"
        "\"system_time_std_dev_sec\":%.6g,"
        "\"avg_queue_wait_time_sec\":%.6g,"
        "\"avg_queue_length\":%.6g,"
        "\"max_queue_length\":%u,"
        "\"jobs_served_by_printer1\":%.0f,"
        "\"printer1_paper_used\":%d,"
        "\"jobs_served_by_printer2\":%.0f,"
        "\"printer2_paper_used\":%d,"
        "\"avg_service_time_p1_sec\":%.6g,"
        "\"avg_service_time_p2_sec\":%.6g,"
        "\"utilization_p1\":%.6g,"
        "\"utilization_p2\":%.6g,"
        "\"paper_refill_events\":%.0f,"
        "\"total_refill_service_time_us\":%.6g,"
        "\"papers_refilled\":%d"
        "}}",
        simulation_time_sec,
        stats->total_jobs_arrived,
        stats->total_jobs_served,
        stats->total_jobs_dropped,
        stats->total_jobs_removed,
        job_arrival_rate,
        job_drop_probability,
        avg_inter_arrival_time,
        avg_system_time,
        system_time_std_dev,
        avg_queue_wait_time,
        avg_queue_length,
        stats->max_job_queue_length,
        stats->jobs_served_by_printer1,
        stats->printer1_paper_used,
        stats->jobs_served_by_printer2,
        stats->printer2_paper_used,
        avg_service_time_p1,
        avg_service_time_p2,
        utilization_p1,
        utilization_p2,
        stats->paper_refill_events,
        stats->total_refill_service_time_us / 1000000.0,
        stats->papers_refilled
    );

    return len;
}

void log_statistics(SimulationStatistics* stats) {
    if (stats == NULL) return;
    
    // Calculate derived statistics (same calculations as publish_statistics)
    double avg_inter_arrival_time = calculate_average_inter_arrival_time(stats);
    double avg_system_time = calculate_average_system_time(stats);
    double avg_queue_wait_time = calculate_average_queue_wait_time(stats);
    double avg_service_time_p1 = calculate_average_service_time_p1(stats);
    double avg_service_time_p2 = calculate_average_service_time_p2(stats);
    double avg_queue_length = calculate_average_queue_length(stats);
    double system_time_std_dev = calculate_system_time_std_dev(stats);
    double utilization_p1 = calculate_system_utilization_p1(stats);
    double utilization_p2 = calculate_system_utilization_p2(stats);
    double job_arrival_rate = calculate_job_arrival_rate(stats);
    double job_drop_probability = calculate_job_drop_probability(stats);
    double simulation_time_sec = stats->simulation_duration_us / 1000000.0;
    
    // Print formatted statistics to stdout
    flockfile(stdout);
    
    printf("\n");
    printf("================= SIMULATION STATISTICS =================\n");
    printf("Simulation Duration:               %.6g sec\n", simulation_time_sec);
    printf("\n");
    printf("--- Job Flow Statistics ---\n");
    printf("Total Jobs Arrived:                %.0f\n", stats->total_jobs_arrived);
    printf("Total Jobs Served:                 %.0f\n", stats->total_jobs_served);
    printf("Total Jobs Dropped:                %.0f\n", stats->total_jobs_dropped);
    printf("Total Jobs Removed:                %.0f\n", stats->total_jobs_removed);
    printf("Job Arrival Rate (λ):              %.6g jobs/sec\n", job_arrival_rate);
    printf("Job Drop Probability:              %.6g (%.2f%%)\n", job_drop_probability, job_drop_probability * 100);
    printf("\n");
    printf("--- Timing Statistics ---\n");
    printf("Average Inter-arrival Time:        %.6g sec\n", avg_inter_arrival_time);
    printf("Average System Time:               %.6g sec\n", avg_system_time);
    printf("System Time Standard Deviation:    %.6g sec\n", system_time_std_dev);
    printf("Average Queue Wait Time:           %.6g sec\n", avg_queue_wait_time);
    printf("\n");
    printf("--- Queue Statistics ---\n");
    printf("Average Queue Length:              %.6g jobs\n", avg_queue_length);
    printf("Maximum Queue Length:              %u jobs\n", stats->max_job_queue_length);
    printf("\n");
    printf("--- Printer Statistics ---\n");
    printf("Jobs Served by Printer 1:          %.0f\n", stats->jobs_served_by_printer1);
    printf("Total Paper Used by Printer 1:     %d\n", stats->printer1_paper_used);
    printf("Jobs Served by Printer 2:          %.0f\n", stats->jobs_served_by_printer2);
    printf("Total Paper Used by Printer 2:     %d\n", stats->printer2_paper_used);
    printf("Avg Service Time (Printer 1):      %.6g sec\n", avg_service_time_p1);
    printf("Avg Service Time (Printer 2):      %.6g sec\n", avg_service_time_p2);
    printf("Utilization (Printer 1):           %.6g%%\n", utilization_p1 * 100);
    printf("Utilization (Printer 2):           %.6g%%\n", utilization_p2 * 100);
    printf("\n");
    printf("--- Paper Management ---\n");
    printf("Paper Refill Events:               %.0f\n", stats->paper_refill_events);
    printf("Total Refill Service Time:         %.6g sec\n", stats->total_refill_service_time_us / 1000000.0);
    printf("Papers Refilled:                   %d\n", stats->papers_refilled);
    printf("=========================================================\n");
    
    funlockfile(stdout);
}

void debug_statistics(const SimulationStatistics* stats) {
    if (stats == NULL) {
        printf("Statistics struct is NULL\n");
        return;
    }
    
    flockfile(stdout);
    printf("\n=== RAW STATISTICS DEBUG ===\n");
    printf("simulation_start_time_us: %lu\n", stats->simulation_start_time_us);
    printf("simulation_duration_us: %lu\n", stats->simulation_duration_us);
    printf("total_jobs_arrived: %.0f\n", stats->total_jobs_arrived);
    printf("total_jobs_served: %.0f\n", stats->total_jobs_served);
    printf("total_jobs_dropped: %.0f\n", stats->total_jobs_dropped);
    printf("total_jobs_removed: %.0f\n", stats->total_jobs_removed);
    printf("total_inter_arrival_time_us: %lu\n", stats->total_inter_arrival_time_us);
    printf("total_system_time_us: %lu\n", stats->total_system_time_us);
    printf("sum_of_system_time_squared_us2: %.0f\n", stats->sum_of_system_time_squared_us2);
    printf("total_queue_wait_time_us: %lu\n", stats->total_queue_wait_time_us);
    printf("area_num_in_job_queue_us: %lu\n", stats->area_num_in_job_queue_us);
    printf("max_job_queue_length: %u\n", stats->max_job_queue_length);
    printf("jobs_served_by_printer1: %.0f\n", stats->jobs_served_by_printer1);
    printf("total_service_time_p1_us: %lu\n", stats->total_service_time_p1_us);
    printf("printer1_paper_empty_time_us: %lu\n", stats->printer1_paper_empty_time_us);
    printf("jobs_served_by_printer2: %.0f\n", stats->jobs_served_by_printer2);
    printf("total_service_time_p2_us: %lu\n", stats->total_service_time_p2_us);
    printf("printer2_paper_empty_time_us: %lu\n", stats->printer2_paper_empty_time_us);
    printf("paper_refill_events: %.0f\n", stats->paper_refill_events);
    printf("total_refill_service_time_us: %lu\n", stats->total_refill_service_time_us);
    printf("papers_refilled: %d\n", stats->papers_refilled);
    printf("==============================\n");
    funlockfile(stdout);
}