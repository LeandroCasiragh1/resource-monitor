// CPU monitor implementation with /proc parsing + legacy stubs
#include "../include/monitors.h"
#include "../include/utils.h"
#include "../include/monitor.h"
#include <unistd.h>

int read_cpu_stats(CPUStats *stats) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        log_error("Failed to open /proc/stat");
        return -1;
    }

    char line[256];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    int parsed = sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                       &stats->user, &stats->nice, &stats->system, &stats->idle,
                       &stats->iowait, &stats->irq, &stats->softirq, &stats->steal);

    fclose(fp);
    return (parsed >= 4) ? 0 : -1;
}

double calculate_cpu_usage(CPUStats *prev, CPUStats *curr) {
    unsigned long long prev_idle = prev->idle + prev->iowait;
    unsigned long long curr_idle = curr->idle + curr->iowait;

    unsigned long long prev_non_idle = prev->user + prev->nice + prev->system +
                                       prev->irq + prev->softirq + prev->steal;
    unsigned long long curr_non_idle = curr->user + curr->nice + curr->system +
                                       curr->irq + curr->softirq + curr->steal;

    unsigned long long prev_total = prev_idle + prev_non_idle;
    unsigned long long curr_total = curr_idle + curr_non_idle;

    unsigned long long total_diff = curr_total - prev_total;
    unsigned long long idle_diff = curr_idle - prev_idle;

    if (total_diff == 0) return 0.0;

    return (100.0 * (total_diff - idle_diff)) / total_diff;
}

int monitor_cpu(int duration_seconds, const char *output_file) {
    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "timestamp,cpu_usage_percent,user,system,idle\n");

    CPUStats prev_stats, curr_stats;
    if (read_cpu_stats(&prev_stats) != 0) {
        fclose(fp);
        return -1;
    }

    for (int i = 0; i < duration_seconds; i++) {
        sleep(1);

        if (read_cpu_stats(&curr_stats) != 0) {
            log_error("Failed to read CPU stats at iteration %d", i);
            continue;
        }

        double usage = calculate_cpu_usage(&prev_stats, &curr_stats);

        char timestamp[64];
        get_timestamp(timestamp, sizeof(timestamp));

        fprintf(fp, "%s,%.2f,%llu,%llu,%llu\n",
                timestamp, usage,
                curr_stats.user, curr_stats.system, curr_stats.idle);

        fflush(fp);

        log_info("CPU Usage: %.2f%%", usage);

        prev_stats = curr_stats;
    }

    fclose(fp);
    log_info("CPU monitoring completed. Data saved to %s", output_file);
    return 0;
}

// Legacy API stubs to satisfy existing code paths (no-op)
void monitor_init(void) {}
void monitor_watch_pid(pid_t pid, int interval_ms) {
    (void)pid; (void)interval_ms;
    log_info("monitor_watch_pid called (stub)");
}
void monitor_stop(void) {}
