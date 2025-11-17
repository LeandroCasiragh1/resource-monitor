#include "../include/experiments.h"
#include "../include/cgroup_v2.h"
#include "../include/monitors.h"
#include "../include/utils.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

static void io_intensive_work(const char *filename, int seconds) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        log_error("Failed to open test file");
        return;
    }
    
    const size_t block_size = 4096; // 4KB blocks
    char buffer[block_size];
    memset(buffer, 0xAB, block_size);
    
    time_t end_time = time(NULL) + seconds;
    unsigned long long total_written = 0;
    
    log_info("Starting I/O intensive work for %d seconds...", seconds);
    
    while (time(NULL) < end_time) {
        ssize_t written = write(fd, buffer, block_size);
        if (written > 0) {
            total_written += written;
        }
    }
    
    close(fd);
    log_info("Wrote %llu bytes total", total_written);
}

int experiment_io_limit(unsigned long limit_mbps, int duration, IOLimitResult *result, const char *output_file) {
    log_info("Starting I/O limit experiment with %lu MB/s limit...", limit_mbps);
    
    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "I/O Limit Experiment Results\n");
    fprintf(fp, "=============================\n");
    fprintf(fp, "Write limit: %lu MB/s\n", limit_mbps);
    fprintf(fp, "Duration: %d seconds\n\n", duration);

    const char *cgroup_name = "io_limit_test";
    const char *test_file = "output/experiments/io_test_file.dat";
    
    if (cgroup_create(cgroup_name) != 0) {
        log_error("Failed to create cgroup");
        fclose(fp);
        return -1;
    }

    // Phase 1: Baseline without limits
    fprintf(fp, "Phase 1: Baseline (no I/O limits)\n");
    log_info("Running baseline test...");
    
    IOStats prev_stats, curr_stats;
    const char *device = "sda"; // Adjust based on your system
    
    if (read_io_stats(device, &prev_stats) != 0) {
        // Try alternative device names
        device = "vda";
        if (read_io_stats(device, &prev_stats) != 0) {
            device = "nvme0n1";
            read_io_stats(device, &prev_stats);
        }
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        io_intensive_work(test_file, duration);
        exit(0);
    }
    
    sleep(1); // Let child start
    
    unsigned long long baseline_bytes = 0;
    
    for (int i = 0; i < duration; i++) {
        sleep(1);
        
        if (read_io_stats(device, &curr_stats) == 0) {
            unsigned long long written = (curr_stats.sectors_written - prev_stats.sectors_written) * 512;
            baseline_bytes += written;
            
            double mb_s = written / (1024.0 * 1024.0);
            fprintf(fp, "  Sample %d: %.2f MB/s\n", i+1, mb_s);
            
            prev_stats = curr_stats;
        }
    }
    
    waitpid(pid, NULL, 0);
    unlink(test_file);
    
    double baseline_mbps = baseline_bytes / (1024.0 * 1024.0 * duration);
    fprintf(fp, "  Average write rate: %.2f MB/s\n\n", baseline_mbps);

    // Phase 2: With I/O limits
    fprintf(fp, "Phase 2: Limited to %lu MB/s\n", limit_mbps);
    log_info("Running limited test...");
    
    // Set I/O limit
    char device_id[64];
    snprintf(device_id, sizeof(device_id), "8:0"); // Major:Minor for sda
    
    unsigned long limit_bps = limit_mbps * 1024 * 1024;
    if (cgroup_set_io_max(cgroup_name, device_id, 0, limit_bps) != 0) {
        log_error("Failed to set I/O limit (continuing anyway)");
    }
    
    if (read_io_stats(device, &prev_stats) != 0) {
        log_error("Failed to read initial I/O stats");
    }
    
    pid = fork();
    if (pid == 0) {
        io_intensive_work(test_file, duration);
        exit(0);
    }
    
    // Add to cgroup
    if (cgroup_add_process(cgroup_name, pid) != 0) {
        log_error("Failed to add process to cgroup");
        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
        cgroup_delete(cgroup_name);
        fclose(fp);
        return -1;
    }
    
    sleep(1);
    
    unsigned long long limited_bytes = 0;
    
    for (int i = 0; i < duration; i++) {
        sleep(1);
        
        if (read_io_stats(device, &curr_stats) == 0) {
            unsigned long long written = (curr_stats.sectors_written - prev_stats.sectors_written) * 512;
            limited_bytes += written;
            
            double mb_s = written / (1024.0 * 1024.0);
            fprintf(fp, "  Sample %d: %.2f MB/s\n", i+1, mb_s);
            
            prev_stats = curr_stats;
        }
    }
    
    waitpid(pid, NULL, 0);
    unlink(test_file);
    
    double limited_mbps = limited_bytes / (1024.0 * 1024.0 * duration);
    double slowdown = 0.0;
    if (baseline_mbps > 0) {
        slowdown = ((baseline_mbps - limited_mbps) / baseline_mbps) * 100.0;
    }
    
    result->limit_bps = limit_bps;
    result->measured_bps = (unsigned long)limited_mbps;
    result->slowdown_percent = slowdown;
    result->duration_seconds = duration;
    
    fprintf(fp, "  Average write rate: %.2f MB/s\n\n", limited_mbps);
    
    fprintf(fp, "Results Summary:\n");
    fprintf(fp, "  Baseline rate: %.2f MB/s\n", baseline_mbps);
    fprintf(fp, "  Limited rate: %.2f MB/s\n", limited_mbps);
    fprintf(fp, "  Target limit: %lu MB/s\n", limit_mbps);
    fprintf(fp, "  Slowdown: %.2f%%\n", slowdown);

    // Cleanup
    cgroup_delete(cgroup_name);
    fclose(fp);
    
    log_info("I/O limit experiment completed");
    return 0;
}
