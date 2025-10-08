#ifndef PAPER_REFILLER_H
#define PAPER_REFILLER_H

// --- Utility functions ---
void debug_refiller(int papers_refilled);

// --- Thread function ---
void* paper_refiller_thread_func(void* arg);

#endif // PAPER_REFILLER_H