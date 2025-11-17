#include "../include/process_monitor.h"
#include "../include/utils.h"
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int process_exists(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d", pid);
    
    struct stat st;
    return (stat(path, &st) == 0);
}

int get_process_name(pid_t pid, char *name, size_t size) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    
    if (fgets(name, size, fp) == NULL) {
        fclose(fp);
        return -1;
    }
    
    // Remove newline
    name[strcspn(name, "\n")] = '\0';
    
    fclose(fp);
    return 0;
}

int read_process_stats(pid_t pid, ProcessStats *stats) {
    if (!process_exists(pid)) {
        log_error("Process %d does not exist", pid);
        return -1;
    }

    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    
    FILE *fp = fopen(path, "r");
    if (!fp) {
        log_error("Failed to open %s", path);
        return -1;
    }

    stats->pid = pid;
    
    int parsed = fscanf(fp, "%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %ld %*d %*u %lu %lu",
                       stats->name, &stats->state, &stats->utime, &stats->stime,
                       &stats->num_threads, &stats->vsize, &stats->rss);
    
    fclose(fp);
    
    if (parsed < 7) {
        log_error("Failed to parse process stats for PID %d", pid);
        return -1;
    }

    // Remove parentheses from name
    if (stats->name[0] == '(') {
        size_t len = strlen(stats->name);
        if (stats->name[len-1] == ')') {
            stats->name[len-1] = '\0';
            memmove(stats->name, stats->name + 1, len);
        }
    }

    // Get memory percentage
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        unsigned long total_mem = si.totalram * si.mem_unit / 1024; // KB
        unsigned long proc_mem = stats->rss * getpagesize() / 1024; // KB
        stats->mem_percent = calculate_percentage(proc_mem, total_mem);
    }

    return 0;
}

double calculate_process_cpu_usage(ProcessStats *prev, ProcessStats *curr, double elapsed) {
    if (elapsed <= 0.0) return 0.0;
    
    unsigned long total_time = (curr->utime + curr->stime) - (prev->utime + prev->stime);
    long clk_tck = sysconf(_SC_CLK_TCK);
    
    if (clk_tck <= 0) return 0.0;
    
    double cpu_time_used = (double)total_time / clk_tck;
    return (cpu_time_used / elapsed) * 100.0;
}

int monitor_process(pid_t pid, int duration_seconds, const char *output_file) {
    if (!process_exists(pid)) {
        log_error("Process %d does not exist", pid);
        return -1;
    }

    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "timestamp,pid,name,state,cpu_percent,mem_percent,vsize_kb,rss_kb,threads\n");

    ProcessStats prev_stats, curr_stats;
    struct timespec prev_time, curr_time;

    if (read_process_stats(pid, &prev_stats) != 0) {
        fclose(fp);
        return -1;
    }
    clock_gettime(CLOCK_MONOTONIC, &prev_time);

    for (int i = 0; i < duration_seconds; i++) {
        sleep(1);

        if (read_process_stats(pid, &curr_stats) != 0) {
            log_error("Process %d terminated or became inaccessible", pid);
            break;
        }
        
        clock_gettime(CLOCK_MONOTONIC, &curr_time);
        double elapsed = get_elapsed_time(&prev_time, &curr_time);
        
        curr_stats.cpu_percent = calculate_process_cpu_usage(&prev_stats, &curr_stats, elapsed);
        
        char timestamp[64];
        get_timestamp(timestamp, sizeof(timestamp));

        fprintf(fp, "%s,%d,%s,%c,%.2f,%.2f,%lu,%lu,%ld\n",
                timestamp, curr_stats.pid, curr_stats.name, curr_stats.state,
                curr_stats.cpu_percent, curr_stats.mem_percent,
                curr_stats.vsize / 1024, curr_stats.rss * getpagesize() / 1024,
                curr_stats.num_threads);
        
        fflush(fp);

        log_info("Process [%d] %s - CPU: %.2f%%, Mem: %.2f%%", 
                 pid, curr_stats.name, curr_stats.cpu_percent, curr_stats.mem_percent);

        prev_stats = curr_stats;
        prev_time = curr_time;
    }

    fclose(fp);
    log_info("Process monitoring completed. Data saved to %s", output_file);
    return 0;
}

int export_process_data_csv(const char *filename, ProcessStats *data, int count) {
    FILE *fp = safe_fopen(filename, "w");
    if (!fp) return -1;

    fprintf(fp, "pid,name,state,cpu_percent,mem_percent,vsize_kb,rss_kb,threads\n");

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d,%s,%c,%.2f,%.2f,%lu,%lu,%ld\n",
                data[i].pid, data[i].name, data[i].state,
                data[i].cpu_percent, data[i].mem_percent,
                data[i].vsize / 1024, data[i].rss * getpagesize() / 1024,
                data[i].num_threads);
    }

    fclose(fp);
    log_info("Process data exported to CSV: %s", filename);
    return 0;
}

int export_process_data_json(const char *filename, ProcessStats *data, int count) {
    FILE *fp = safe_fopen(filename, "w");
    if (!fp) return -1;

    fprintf(fp, "[\n");

    for (int i = 0; i < count; i++) {
        fprintf(fp, "  {\n");
        fprintf(fp, "    \"pid\": %d,\n", data[i].pid);
        fprintf(fp, "    \"name\": \"%s\",\n", data[i].name);
        fprintf(fp, "    \"state\": \"%c\",\n", data[i].state);
        fprintf(fp, "    \"cpu_percent\": %.2f,\n", data[i].cpu_percent);
        fprintf(fp, "    \"mem_percent\": %.2f,\n", data[i].mem_percent);
        fprintf(fp, "    \"vsize_kb\": %lu,\n", data[i].vsize / 1024);
        fprintf(fp, "    \"rss_kb\": %lu,\n", data[i].rss * getpagesize() / 1024);
        fprintf(fp, "    \"threads\": %ld\n", data[i].num_threads);
        fprintf(fp, "  }%s\n", (i < count - 1) ? "," : "");
    }

    fprintf(fp, "]\n");

    fclose(fp);
    log_info("Process data exported to JSON: %s", filename);
    return 0;
}
