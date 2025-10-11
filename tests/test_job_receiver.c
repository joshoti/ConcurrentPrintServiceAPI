#include <stdio.h>
#include <string.h>
#include "test_utils.h"
#include "job_receiver.h"

int test_job_init(Job* job) {
    int failed = 0;
    if (!init_job(job, 1, 1000000, 10)) {
        printf("Test failed: init_job returned FALSE\n");
        return 1;
    }
    if (job->id != 1 || job->inter_arrival_time_us != 1000000 || job->papers_required != 10) {
        printf("Test failed: init_job did not set fields correctly\n");
        failed = 1;
    } else {
        printf("Test passed: init_job set fields correctly\n");
    }
    return failed;
}

int test_debug_job(Job* job) {
    printf("Testing debug_job output:\n");
    debug_job(job);
    // Manual verification needed for debug output
    return 0;
}

int main() {
    char test_name[] = "JOB";
    print_test_start(test_name);
    int failed_tests = 0;

    Job job;
    failed_tests += test_job_init(&job);
    failed_tests += test_debug_job(&job);

    print_test_end(test_name, failed_tests);
    return 0;
}

