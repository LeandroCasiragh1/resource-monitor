#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// Process statistics structure
typedef struct {
    pid_t pid;
    char name[256];
    char state;
    unsigned long utime;
    unsigned long stime;
    unsigned long vsize;
    unsigned long rss;
    long num_threads;
    double cpu_percent;
    double mem_percent;
} ProcessStats;

// Process monitoring functions
int read_process_stats(pid_t pid, ProcessStats *stats);
int monitor_process(pid_t pid, int duration_seconds, const char *output_file);
int export_process_data_csv(const char *filename, ProcessStats *data, int count);
int export_process_data_json(const char *filename, ProcessStats *data, int count);

// Process utilities
int process_exists(pid_t pid);
int get_process_name(pid_t pid, char *name, size_t size);
double calculate_process_cpu_usage(ProcessStats *prev, ProcessStats *curr, double elapsed);

#endif // PROCESS_MONITOR_H
