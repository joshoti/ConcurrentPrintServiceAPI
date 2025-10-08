#ifndef PRINTER_H
#define PRINTER_H

// --- Utility functions ---
void debug_printer(int printer_id, int printed_count);

// --- Thread function ---
void* printer_thread_func(void* arg);

#endif // PRINTER_H