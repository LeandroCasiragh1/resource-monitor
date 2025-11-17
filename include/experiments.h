#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Experiment result structures
typedef struct {
    double overhead_percent;
    double baseline_duration;
    double monitored_duration;
    int samples;
} OverheadResult;

typedef struct {
    double throttled_usage;
    double unthrottled_usage;
    double throttle_percent;
    int duration_seconds;
} CPUThrottleResult;

typedef struct {
    unsigned long limit_bytes;
    unsigned long peak_usage;
    int oom_occurred;
    int allocation_count;
} MemoryLimitResult;

typedef struct {
    unsigned long limit_bps;
    unsigned long measured_bps;
    double slowdown_percent;
    int duration_seconds;
} IOLimitResult;

typedef struct {
    int namespace_type;
    int isolation_verified;
    char test_details[256];
} NamespaceResult;

// Experiment functions
int experiment_overhead(OverheadResult *result, const char *output_file);
int experiment_cpu_throttling(int throttle_percent, int duration, CPUThrottleResult *result, const char *output_file);
int experiment_memory_limit(unsigned long limit_mb, MemoryLimitResult *result, const char *output_file);
int experiment_io_limit(unsigned long limit_mbps, int duration, IOLimitResult *result, const char *output_file);
int experiment_namespace_isolation(int ns_type, NamespaceResult *result, const char *output_file);

// Utility functions for experiments
void save_experiment_results(const char *filename, const char *experiment_name, void *result, int result_type);
void print_experiment_summary(void);

#endif // EXPERIMENTS_H
