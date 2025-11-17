#include "../include/experiments.h"
#include "../include/cgroup_v2.h"
#include "../include/monitors.h"
#include "../include/utils.h"
#include <unistd.h>
#include <sys/wait.h>

static void cpu_intensive_work(int seconds) {
    time_t end_time = time(NULL) + seconds;
    volatile long sum = 0;
    
    while (time(NULL) < end_time) {
        for (int i = 0; i < 1000000; i++) {
            sum += i * i;
        }
    }
}

int experiment_cpu_throttling(int throttle_percent, int duration, CPUThrottleResult *result, const char *output_file) {
    log_info("Starting CPU throttling experiment with %d%% limit...", throttle_percent);
    
    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "CPU Throttling Experiment Results\n");
    fprintf(fp, "==================================\n");
    fprintf(fp, "Throttle limit: %d%%\n", throttle_percent);
    fprintf(fp, "Duration: %d seconds\n\n", duration);

    // Create cgroup for experiment
    const char *cgroup_name = "cpu_throttle_test";
    
    if (cgroup_create(cgroup_name) != 0) {
        log_error("Failed to create cgroup");
        fclose(fp);
        return -1;
    }

    // Phase 1: Unthrottled baseline
    fprintf(fp, "Phase 1: Unthrottled baseline\n");
    log_info("Running unthrottled baseline for %d seconds...", duration);
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: CPU-intensive work
        cpu_intensive_work(duration);
        exit(0);
    }
    
    // Parent: Monitor CPU usage
    sleep(1); // Let child start
    
    char baseline_file[256];
    snprintf(baseline_file, sizeof(baseline_file), "output/experiments/cpu_baseline_%d.csv", getpid());
    
    CPUStats prev_stats, curr_stats;
    read_cpu_stats(&prev_stats);
    
    double unthrottled_total = 0.0;
    int samples = 0;
    
    for (int i = 0; i < duration; i++) {
        sleep(1);
        read_cpu_stats(&curr_stats);
        double usage = calculate_cpu_usage(&prev_stats, &curr_stats);
        unthrottled_total += usage;
        samples++;
        fprintf(fp, "  Sample %d: %.2f%%\n", i+1, usage);
        prev_stats = curr_stats;
    }
    
    waitpid(pid, NULL, 0);
    result->unthrottled_usage = unthrottled_total / samples;
    
    fprintf(fp, "  Average CPU usage: %.2f%%\n\n", result->unthrottled_usage);

    // Phase 2: Throttled
    fprintf(fp, "Phase 2: Throttled to %d%%\n", throttle_percent);
    log_info("Running throttled test...");
    
    // Set CPU limit (quota/period)
    long period = 100000; // 100ms
    long quota = period * throttle_percent / 100;
    
    if (cgroup_set_cpu_max(cgroup_name, quota, period) != 0) {
        log_error("Failed to set CPU limit");
        cgroup_delete(cgroup_name);
        fclose(fp);
        return -1;
    }
    
    pid = fork();
    if (pid == 0) {
        // Child process: CPU-intensive work
        cpu_intensive_work(duration);
        exit(0);
    }
    
    // Add child to cgroup
    if (cgroup_add_process(cgroup_name, pid) != 0) {
        log_error("Failed to add process to cgroup");
        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
        cgroup_delete(cgroup_name);
        fclose(fp);
        return -1;
    }
    
    sleep(1); // Let child start
    
    read_cpu_stats(&prev_stats);
    double throttled_total = 0.0;
    samples = 0;
    
    for (int i = 0; i < duration; i++) {
        sleep(1);
        read_cpu_stats(&curr_stats);
        double usage = calculate_cpu_usage(&prev_stats, &curr_stats);
        throttled_total += usage;
        samples++;
        fprintf(fp, "  Sample %d: %.2f%%\n", i+1, usage);
        prev_stats = curr_stats;
    }
    
    waitpid(pid, NULL, 0);
    result->throttled_usage = throttled_total / samples;
    result->throttle_percent = throttle_percent;
    result->duration_seconds = duration;
    
    fprintf(fp, "  Average CPU usage: %.2f%%\n\n", result->throttled_usage);
    
    fprintf(fp, "Results Summary:\n");
    fprintf(fp, "  Unthrottled average: %.2f%%\n", result->unthrottled_usage);
    fprintf(fp, "  Throttled average: %.2f%%\n", result->throttled_usage);
    fprintf(fp, "  Target limit: %d%%\n", throttle_percent);
    fprintf(fp, "  Effective reduction: %.2f%%\n", 
            result->unthrottled_usage - result->throttled_usage);

    // Cleanup
    cgroup_delete(cgroup_name);
    fclose(fp);
    
    log_info("CPU throttling experiment completed");
    return 0;
}
