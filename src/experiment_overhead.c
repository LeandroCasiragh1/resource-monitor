#include "../include/experiments.h"
#include "../include/monitors.h"
#include "../include/utils.h"
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int experiment_overhead(OverheadResult *result, const char *output_file) {
    log_info("Starting overhead experiment...");
    
    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "Monitoring Overhead Experiment Results\n");
    fprintf(fp, "======================================\n\n");

    const int SAMPLES = 10;
    const int ITERATIONS = 100000;
    
    struct timespec start, end;
    double baseline_times[SAMPLES];
    double monitored_times[SAMPLES];

    // Baseline: CPU-intensive task without monitoring
    fprintf(fp, "Phase 1: Baseline measurements (no monitoring)\n");
    for (int i = 0; i < SAMPLES; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        // CPU-intensive work
        volatile long sum = 0;
        for (int j = 0; j < ITERATIONS; j++) {
            sum += j * j;
        }
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        baseline_times[i] = get_elapsed_time(&start, &end);
        
        fprintf(fp, "  Sample %d: %.6f seconds\n", i+1, baseline_times[i]);
        log_info("Baseline sample %d: %.6f seconds", i+1, baseline_times[i]);
    }

    // With monitoring: Same task while reading CPU stats
    fprintf(fp, "\nPhase 2: Monitored measurements (with CPU monitoring)\n");
    for (int i = 0; i < SAMPLES; i++) {
        CPUStats stats;
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        // CPU-intensive work with periodic monitoring
        volatile long sum = 0;
        for (int j = 0; j < ITERATIONS; j++) {
            sum += j * j;
            if (j % 10000 == 0) {
                read_cpu_stats(&stats);
            }
        }
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        monitored_times[i] = get_elapsed_time(&start, &end);
        
        fprintf(fp, "  Sample %d: %.6f seconds\n", i+1, monitored_times[i]);
        log_info("Monitored sample %d: %.6f seconds", i+1, monitored_times[i]);
    }

    // Calculate averages
    double baseline_avg = 0.0, monitored_avg = 0.0;
    for (int i = 0; i < SAMPLES; i++) {
        baseline_avg += baseline_times[i];
        monitored_avg += monitored_times[i];
    }
    baseline_avg /= SAMPLES;
    monitored_avg /= SAMPLES;

    double overhead = ((monitored_avg - baseline_avg) / baseline_avg) * 100.0;

    result->baseline_duration = baseline_avg;
    result->monitored_duration = monitored_avg;
    result->overhead_percent = overhead;
    result->samples = SAMPLES;

    fprintf(fp, "\nResults Summary:\n");
    fprintf(fp, "  Baseline average: %.6f seconds\n", baseline_avg);
    fprintf(fp, "  Monitored average: %.6f seconds\n", monitored_avg);
    fprintf(fp, "  Overhead: %.2f%%\n", overhead);

    fclose(fp);
    
    log_info("Overhead experiment completed: %.2f%% overhead", overhead);
    return 0;
}
