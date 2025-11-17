#ifndef MONITORS_H
#define MONITORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// CPU Monitor structures
typedef struct {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
} CPUStats;

typedef struct {
    double usage_percent;
    unsigned long long total;
    unsigned long long idle;
} CPUUsage;

// Memory Monitor structures
typedef struct {
    unsigned long total;
    unsigned long free;
    unsigned long available;
    unsigned long buffers;
    unsigned long cached;
    unsigned long swap_total;
    unsigned long swap_free;
    double usage_percent;
} MemoryStats;

// I/O Monitor structures
typedef struct {
    char device[32];
    unsigned long long reads_completed;
    unsigned long long reads_merged;
    unsigned long long sectors_read;
    unsigned long long time_reading;
    unsigned long long writes_completed;
    unsigned long long writes_merged;
    unsigned long long sectors_written;
    unsigned long long time_writing;
    unsigned long long ios_in_progress;
    unsigned long long time_io;
    unsigned long long weighted_time_io;
} IOStats;

// Network Monitor structures
typedef struct {
    char interface[32];
    unsigned long long rx_bytes;
    unsigned long long rx_packets;
    unsigned long long rx_errors;
    unsigned long long rx_dropped;
    unsigned long long tx_bytes;
    unsigned long long tx_packets;
    unsigned long long tx_errors;
    unsigned long long tx_dropped;
} NetworkStats;

// CPU Monitor functions
int read_cpu_stats(CPUStats *stats);
double calculate_cpu_usage(CPUStats *prev, CPUStats *curr);
int monitor_cpu(int duration_seconds, const char *output_file);

// Memory Monitor functions
int read_memory_stats(MemoryStats *stats);
int monitor_memory(int duration_seconds, const char *output_file);

// I/O Monitor functions
int read_io_stats(const char *device, IOStats *stats);
int monitor_io(const char *device, int duration_seconds, const char *output_file);

// Network Monitor functions
int read_network_stats(const char *interface, NetworkStats *stats);
int monitor_network(const char *interface, int duration_seconds, const char *output_file);

#endif // MONITORS_H
