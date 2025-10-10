#include <stdio.h>
#include <string.h>
#include "common.h"
#include "preprocessing.h"
#include "test_utils.h"

int test_process_args() {
    int failed = 0;
    char *argv[] = {
        "program_name",
        "-num", "5",
        "-q", "10",
        "-p_cap", "100",
        "-arr", "0.1",
        "-s", "0.2",
        "-ref", "0.3"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    SimulationParameters params;

    if (process_args(argc, argv, &params)) {
        printf("Test passed: num_jobs: %d queue_capacity: %d printer_paper_capacity: %d arrival_time: %gus service_time: %gus refill_time: %gus\n",
               params.num_jobs,
               params.queue_capacity,
               params.printer_paper_capacity,
               params.job_arrival_time_us,
               params.job_printing_time_us,
               params.refill_time_us);
    } else {
        printf("Test failed\n");
        failed = 1;
    }
    return failed;
}

int test_bad_args() {
    int failed = 0;
    char *argv[] = {
        "program_name",
        "-num", "-5",  // Invalid negative number
        "-q", "10",
        "-p_cap", "100",
        "-arr", "0.1",
        "-s", "0.2",
        "-ref", "0.3"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    SimulationParameters params;

    if (!process_args(argc, argv, &params)) {
        printf("Test passed: Detected invalid argument\n");
    } else {
        printf("Test failed: Did not detect invalid argument\n");
        failed = 1;
    }
    return failed;
}

int main() {
    char test_name[] = "PREPROCESSING";
    print_test_start(test_name);
    int failed_tests = 0;
    failed_tests += test_process_args();
    failed_tests += test_bad_args();
    print_test_end(test_name, failed_tests);
    return 0;
}