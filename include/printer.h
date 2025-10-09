#ifndef PRINTER_H
#define PRINTER_H

// --- Printer structure ---
typedef struct {
    int id; // Unique identifier for the printer
    int paper_count; // Current number of papers in the printer
    int capacity; // Maximum paper capacity of the printer
    int printed_count; // Total number of jobs printed by this printer
} Printer;

// --- Utility functions ---
void debug_printer(int printer_id, int printed_count);

// --- Thread function ---
void* printer_thread_func(void* arg);

#endif // PRINTER_H