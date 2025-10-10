#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "common.h"
#include "preprocessing.h"

int g_debug = 0;

void usage() {
    fprintf(stderr, "usage: ./cli [-debug] [-num num_jobs] [-q queue_capacity] [-p_cap printer_paper_capacity] [-arr arrival_rate] [-s service_rate] [-ref refill_rate] [-papers_lower papers_required_lower_bound] [-papers_upper papers_required_upper_bound]\n");
}

int random_between(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

void swap_bounds(int* lower, int* upper) {
    int temp = *lower;
    *lower = (int)fmin(*upper, *lower);
    *upper = (int)fmax(temp, *upper);
}

int is_positive_double(const char* str, double value) {
    if (value <= 0) {
        fprintf(stderr, "Error: %s must be a positive number.\n", str);
        return FALSE;
    }
    return TRUE;
}

int is_positive_integer(const char* str, int value) {
    if (value <= 0) {
        fprintf(stderr, "Error: %s must be a positive integer.\n", str);
        return FALSE;
    }
    return TRUE;
}

int process_args(int argc, char *argv[], SimulationParameters* params) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-num") == 0) {
            params->num_jobs = atoi(argv[++i]);
            if (!is_positive_integer("num_jobs", params->num_jobs)) return FALSE;
        } else if (strcmp(argv[i], "-q") == 0) {
            params->queue_capacity = atoi(argv[++i]);
            if (!is_positive_integer("queue_capacity", params->queue_capacity)) return FALSE;
        } else if (strcmp(argv[i], "-papers_lower") == 0) {
            params->papers_required_lower_bound = atoi(argv[++i]);
            if (!is_positive_integer("papers_required_lower_bound", params->papers_required_lower_bound)) return FALSE;
        } else if (strcmp(argv[i], "-papers_upper") == 0) {
            params->papers_required_upper_bound = atoi(argv[++i]);
            if (!is_positive_integer("papers_required_upper_bound", params->papers_required_upper_bound)) return FALSE;
        } else if (strcmp(argv[i], "-p_cap") == 0) {
            params->printer_paper_capacity = atoi(argv[++i]);
            if (!is_positive_integer("printer_paper_capacity", params->printer_paper_capacity)) return FALSE;
        } else if (strcmp(argv[i], "-arr") == 0) {
            double arrival_rate = atof(argv[++i]);
            if (!is_positive_double("arrival_rate", arrival_rate)) return FALSE;
            params->job_arrival_time_us = (int)(1000000.0 / arrival_rate);
        } else if (strcmp(argv[i], "-s") == 0) {
            double service_rate = atof(argv[++i]);
            if (!is_positive_double("service_rate", service_rate)) return FALSE;
            params->printing_rate = service_rate;
        } else if (strcmp(argv[i], "-ref") == 0) {
            params->refill_rate = atof(argv[++i]);
            if (!is_positive_double("refill_rate", params->refill_rate)) return FALSE;
        } else if (strcmp(argv[i], "-debug") == 0) {
            g_debug = 1;
        } else {
            fprintf(stderr, "Error: unrecognized argument %s.\n", argv[i]);
            usage();
            return FALSE;
        }
        swap_bounds(&params->papers_required_lower_bound, &params->papers_required_upper_bound);
    }
    return TRUE;
}