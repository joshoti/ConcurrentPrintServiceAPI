#ifndef SIGNAL_CATCHER_H
#define SIGNAL_CATCHER_H

// --- Utility functions ---
void empty_queue_if_terminating();

// --- Thread function ---
void* sig_int_catching_thread_func(void* arg);

#endif // SIGNAL_CATCHER_H