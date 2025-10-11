#ifndef TIMEUTILS_H
#define TIMEUTILS_H

/**
 * @file timeutils.h
 * @brief Utility functions for time management in microseconds and milliseconds.
 *
 * This header provides functions to get the current time in microseconds,
 * convert time from microseconds to milliseconds and microseconds, and
 * calculate wake-up times based on a given delay in milliseconds.
 *
 * All time calculations are based on the `gettimeofday` function.
 */

/**
 * @brief Get the current time in microseconds since the Epoch.
 *
 * @return Current time in microseconds.
 */
unsigned long get_time_in_us();

/**
 * @brief Convert time from microseconds to milliseconds and microseconds.
 *
 * @param current_time_us Time in microseconds.
 * @param time_ms Pointer to store the converted time in milliseconds.
 * @param time_us Pointer to store the remaining microseconds.
 *
 * For example, given `current_time_us` = 9253307, sets `*time_ms` to 9253 and `*time_us` to 307.
 */
void time_in_us_to_ms(unsigned long current_time_us, int* time_ms, int* time_us);

/**
 * @brief Calculate the wake-up time based on a delay in milliseconds.
 *
 * @param time_ms Delay time in milliseconds.
 * @return A timespec struct representing the wake-up time.
 */
struct timespec get_wake_up_time(int time_ms);

/** 
 * Format string for time output: "milliseconds.microseconds"
 */
extern const char time_format[];

#endif // TIMEUTILS_H