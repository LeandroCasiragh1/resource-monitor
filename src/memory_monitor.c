#include "../include/monitors.h"
#include "../include/utils.h"
#include <unistd.h>

int read_memory_stats(MemoryStats *stats) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        log_error("Failed to open /proc/meminfo");
        return -1;
    }

    char line[256];
    memset(stats, 0, sizeof(MemoryStats));

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%lu", &stats->total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%lu", &stats->free);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line + 13, "%lu", &stats->available);
        } else if (strncmp(line, "Buffers:", 8) == 0) {
            sscanf(line + 8, "%lu", &stats->buffers);
        } else if (strncmp(line, "Cached:", 7) == 0) {
            sscanf(line + 7, "%lu", &stats->cached);
        } else if (strncmp(line, "SwapTotal:", 10) == 0) {
            sscanf(line + 10, "%lu", &stats->swap_total);
        } else if (strncmp(line, "SwapFree:", 9) == 0) {
            sscanf(line + 9, "%lu", &stats->swap_free);
        }
    }

    fclose(fp);

    if (stats->total > 0) {
        unsigned long used = stats->total - stats->available;
        stats->usage_percent = calculate_percentage(used, stats->total);
    }

    return 0;
}

int monitor_memory(int duration_seconds, const char *output_file) {
    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "timestamp,total_kb,used_kb,free_kb,available_kb,usage_percent,cached_kb,buffers_kb,swap_total_kb,swap_free_kb\n");

    for (int i = 0; i < duration_seconds; i++) {
        MemoryStats stats;
        if (read_memory_stats(&stats) != 0) {
            log_error("Failed to read memory stats at iteration %d", i);
            sleep(1);
            continue;
        }

        unsigned long used = stats.total - stats.available;

        char timestamp[64];
        get_timestamp(timestamp, sizeof(timestamp));

        fprintf(fp, "%s,%lu,%lu,%lu,%lu,%.2f,%lu,%lu,%lu,%lu\n",
                timestamp, stats.total, used, stats.free, stats.available,
                stats.usage_percent, stats.cached, stats.buffers,
                stats.swap_total, stats.swap_free);

        fflush(fp);

        log_info("Memory Usage: %.2f%% (%lu/%lu KB)",
                 stats.usage_percent, used, stats.total);

        sleep(1);
    }

    fclose(fp);
    log_info("Memory monitoring completed. Data saved to %s", output_file);
    return 0;
}
