#include "../include/experiments.h"
#include "../include/cgroup_v2.h"
#include "../include/utils.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

static volatile int allocation_failed = 0;

static void memory_allocator(unsigned long target_mb) {
    const size_t chunk_size = 1024 * 1024; // 1 MB chunks
    char **allocations = malloc(target_mb * sizeof(char*));
    int allocated = 0;
    
    log_info("Allocating memory in 1MB chunks up to %lu MB...", target_mb);
    
    for (unsigned long i = 0; i < target_mb; i++) {
        allocations[i] = malloc(chunk_size);
        
        if (allocations[i] == NULL) {
            allocation_failed = 1;
            log_error("Memory allocation failed at %d MB", allocated);
            break;
        }
        
        // Touch the memory to ensure it's actually allocated
        memset(allocations[i], 0xAB, chunk_size);
        allocated++;
        
        if (i % 10 == 0) {
            log_info("Allocated %d MB so far...", allocated);
        }
        
        usleep(100000); // 100ms between allocations
    }
    
    log_info("Total allocated: %d MB", allocated);
    
    // Hold the memory for a bit
    sleep(5);
    
    // Cleanup
    for (int i = 0; i < allocated; i++) {
        free(allocations[i]);
    }
    free(allocations);
}

int experiment_memory_limit(unsigned long limit_mb, MemoryLimitResult *result, const char *output_file) {
    log_info("Starting memory limit experiment with %lu MB limit...", limit_mb);
    
    FILE *fp = safe_fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "Memory Limit Experiment Results\n");
    fprintf(fp, "================================\n");
    fprintf(fp, "Memory limit: %lu MB\n\n", limit_mb);

    const char *cgroup_name = "memory_limit_test";
    
    if (cgroup_create(cgroup_name) != 0) {
        log_error("Failed to create cgroup");
        fclose(fp);
        return -1;
    }

    // Set memory limit
    unsigned long limit_bytes = limit_mb * 1024 * 1024;
    if (cgroup_set_memory_max(cgroup_name, limit_bytes) != 0) {
        log_error("Failed to set memory limit");
        cgroup_delete(cgroup_name);
        fclose(fp);
        return -1;
    }

    fprintf(fp, "Phase 1: Memory allocation test\n");
    fprintf(fp, "Attempting to allocate %lu MB (limit: %lu MB)\n\n", limit_mb + 50, limit_mb);
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: try to allocate more than the limit
        memory_allocator(limit_mb + 50);
        exit(allocation_failed ? 1 : 0);
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
    
    // Monitor memory usage
    unsigned long peak_usage = 0;
    int oom_killed = 0;
    int status;
    
    fprintf(fp, "Monitoring memory usage:\n");
    
    for (int i = 0; i < 30; i++) {
        sleep(1);
        
        unsigned long current_usage;
        if (cgroup_get_memory_usage(cgroup_name, &current_usage) == 0) {
            unsigned long usage_mb = current_usage / (1024 * 1024);
            fprintf(fp, "  Sample %d: %lu MB\n", i+1, usage_mb);
            
            if (usage_mb > peak_usage) {
                peak_usage = usage_mb;
            }
        }
        
        // Check if process is still running
        if (waitpid(pid, &status, WNOHANG) != 0) {
            if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
                oom_killed = 1;
                fprintf(fp, "\nProcess was OOM killed by kernel!\n");
                log_info("Process was OOM killed");
            } else if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) == 1) {
                    fprintf(fp, "\nProcess hit allocation failure (before OOM kill)\n");
                } else {
                    fprintf(fp, "\nProcess completed successfully\n");
                }
            }
            break;
        }
    }
    
    // If still running, terminate it
    if (kill(pid, 0) == 0) {
        kill(pid, SIGTERM);
        waitpid(pid, NULL, 0);
    }
    
    result->limit_bytes = limit_bytes;
    result->peak_usage = peak_usage * 1024 * 1024;
    result->oom_occurred = oom_killed;
    result->allocation_count = peak_usage; // MB allocated
    
    fprintf(fp, "\nResults Summary:\n");
    fprintf(fp, "  Memory limit: %lu MB\n", limit_mb);
    fprintf(fp, "  Peak usage: %lu MB\n", peak_usage);
    fprintf(fp, "  OOM occurred: %s\n", oom_killed ? "Yes" : "No");
    fprintf(fp, "  Limit enforced: %s\n", 
            (peak_usage <= limit_mb + 5) ? "Yes" : "No"); // 5MB tolerance

    // Cleanup
    cgroup_delete(cgroup_name);
    fclose(fp);
    
    log_info("Memory limit experiment completed");
    return 0;
}
