#include "../include/monitors.h"
#include "../include/utils.h"
#include <unistd.h>

int read_io_stats(const char *device, IOStats *stats) {
    FILE *fp = fopen("/proc/diskstats", "r");
    if (!fp) {
        log_error("Failed to open /proc/diskstats");
        return -1;
    }

    char line[512];
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        unsigned int major, minor;
        char dev_name[32];

        int parsed = sscanf(line, "%u %u %s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                          &major, &minor, dev_name,
                          &stats->reads_completed, &stats->reads_merged,
                          &stats->sectors_read, &stats->time_reading,
                          &stats->writes_completed, &stats->writes_merged,
                          &stats->sectors_written, &stats->time_writing,
                          &stats->ios_in_progress, &stats->time_io,
                          &stats->weighted_time_io);

        if (parsed >= 14 && strcmp(dev_name, device) == 0) {
            strncpy(stats->device, dev_name, sizeof(stats->device) - 1);
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found ? 0 : -1;
}

int monitor_io(const char *device, int duration_seconds, const char *output_file) {
    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "timestamp,reads_completed,reads_merged,sectors_read,writes_completed,writes_merged,sectors_written,read_rate_kb_s,write_rate_kb_s\n");

    IOStats prev_stats, curr_stats;
    if (read_io_stats(device, &prev_stats) != 0) {
        log_error("Failed to read initial I/O stats for device %s", device);
        fclose(fp);
        return -1;
    }

    for (int i = 0; i < duration_seconds; i++) {
        sleep(1);

        if (read_io_stats(device, &curr_stats) != 0) {
            log_error("Failed to read I/O stats at iteration %d", i);
            continue;
        }

        unsigned long long read_sectors = curr_stats.sectors_read - prev_stats.sectors_read;
        unsigned long long write_sectors = curr_stats.sectors_written - prev_stats.sectors_written;

        double read_kb_s = (read_sectors * 512) / 1024.0;
        double write_kb_s = (write_sectors * 512) / 1024.0;

        char timestamp[64];
        get_timestamp(timestamp, sizeof(timestamp));

        fprintf(fp, "%s,%llu,%llu,%llu,%llu,%llu,%llu,%.2f,%.2f\n",
                timestamp,
                curr_stats.reads_completed, curr_stats.reads_merged, curr_stats.sectors_read,
                curr_stats.writes_completed, curr_stats.writes_merged, curr_stats.sectors_written,
                read_kb_s, write_kb_s);

        fflush(fp);

        log_info("I/O [%s] - Read: %.2f KB/s, Write: %.2f KB/s",
                 device, read_kb_s, write_kb_s);

        prev_stats = curr_stats;
    }

    fclose(fp);
    log_info("I/O monitoring completed. Data saved to %s", output_file);
    return 0;
}
