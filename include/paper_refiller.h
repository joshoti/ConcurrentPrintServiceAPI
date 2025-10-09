#ifndef PAPER_REFILLER_H
#define PAPER_REFILLER_H

// --- Utility functions ---
/**
 * @brief Prints paper refiller debug information.
 *
 * @param papers_supplied The number of papers supplied during the refill.
 */
void debug_refiller(int papers_supplied);

// --- Thread function ---
/**
 * @brief The main function for the paper refiller thread.
 *
 * @param arg The argument passed to the thread (not used).
 * @return NULL
 */
void* paper_refiller_thread_func(void* arg);

#endif // PAPER_REFILLER_H