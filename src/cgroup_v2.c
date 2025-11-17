#include "../include/cgroup_v2.h"
#include "../include/utils.h"
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

char* cgroup_get_full_path(const char *name) {
    static char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s", CGROUP_BASE_PATH, name);
    return path;
}

int cgroup_is_v2_available(void) {
    struct stat st;
    char path[MAX_PATH_LEN];
    
    snprintf(path, sizeof(path), "%s/cgroup.controllers", CGROUP_BASE_PATH);
    return (stat(path, &st) == 0);
}

int cgroup_exists(const char *name) {
    struct stat st;
    char *path = cgroup_get_full_path(name);
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

// Commented out to avoid collision with cgroup_manager.c
/*
int cgroup_create(const char *name) {
    if (cgroup_exists(name)) {
        log_info("Cgroup %s already exists", name);
        return 0;
    }

    char *path = cgroup_get_full_path(name);
    
    if (mkdir(path, 0755) == -1) {
        log_error("Failed to create cgroup %s: %s", name, strerror(errno));
        return -1;
    }

    log_info("Created cgroup: %s", name);
    return 0;
}
*/

int cgroup_delete(const char *name) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char *path = cgroup_get_full_path(name);
    
    if (rmdir(path) == -1) {
        log_error("Failed to delete cgroup %s: %s", name, strerror(errno));
        return -1;
    }

    log_info("Deleted cgroup: %s", name);
    return 0;
}

int cgroup_add_process(const char *name, pid_t pid) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/cgroup.procs", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "w");
    if (!fp) {
        log_error("Failed to open cgroup.procs for %s: %s", name, strerror(errno));
        return -1;
    }

    fprintf(fp, "%d\n", pid);
    fclose(fp);

    log_info("Added process %d to cgroup %s", pid, name);
    return 0;
}

int cgroup_remove_process(const char *name, pid_t pid) {
    // Move process back to root cgroup
    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/cgroup.procs", CGROUP_BASE_PATH);

    FILE *fp = fopen(path, "w");
    if (!fp) {
        log_error("Failed to open root cgroup.procs: %s", strerror(errno));
        return -1;
    }

    fprintf(fp, "%d\n", pid);
    fclose(fp);

    log_info("Removed process %d from cgroup %s", pid, name);
    return 0;
}

int cgroup_set_cpu_max(const char *name, long quota, long period) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/cpu.max", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "w");
    if (!fp) {
        log_error("Failed to open cpu.max for %s: %s", name, strerror(errno));
        return -1;
    }

    if (quota < 0) {
        fprintf(fp, "max %ld\n", period);
    } else {
        fprintf(fp, "%ld %ld\n", quota, period);
    }
    
    fclose(fp);

    log_info("Set CPU limit for %s: quota=%ld, period=%ld", name, quota, period);
    return 0;
}

int cgroup_get_cpu_usage(const char *name, unsigned long long *usage) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/cpu.stat", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        log_error("Failed to open cpu.stat for %s: %s", name, strerror(errno));
        return -1;
    }

    char line[256];
    int found = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (parse_line_value_ull(line, "usage_usec", usage)) {
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found ? 0 : -1;
}

int cgroup_set_cpu_weight(const char *name, int weight) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    if (weight < 1 || weight > 10000) {
        log_error("Invalid CPU weight %d (must be 1-10000)", weight);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/cpu.weight", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "w");
    if (!fp) {
        log_error("Failed to open cpu.weight for %s: %s", name, strerror(errno));
        return -1;
    }

    fprintf(fp, "%d\n", weight);
    fclose(fp);

    log_info("Set CPU weight for %s: %d", name, weight);
    return 0;
}

// Commented out to avoid collision with cgroup_manager.c
/*
int cgroup_set_memory_max(const char *name, unsigned long bytes) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/memory.max", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "w");
    if (!fp) {
        log_error("Failed to open memory.max for %s: %s", name, strerror(errno));
        return -1;
    }

    fprintf(fp, "%lu\n", bytes);
    fclose(fp);

    log_info("Set memory limit for %s: %lu bytes", name, bytes);
    return 0;
}
*/

int cgroup_get_memory_usage(const char *name, unsigned long *usage) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/memory.current", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        log_error("Failed to open memory.current for %s: %s", name, strerror(errno));
        return -1;
    }

    if (fscanf(fp, "%lu", usage) != 1) {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int cgroup_set_memory_high(const char *name, unsigned long bytes) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/memory.high", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "w");
    if (!fp) {
        log_error("Failed to open memory.high for %s: %s", name, strerror(errno));
        return -1;
    }

    fprintf(fp, "%lu\n", bytes);
    fclose(fp);

    log_info("Set memory high watermark for %s: %lu bytes", name, bytes);
    return 0;
}

int cgroup_set_io_max(const char *name, const char *device, unsigned long rbps, unsigned long wbps) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/io.max", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "w");
    if (!fp) {
        log_error("Failed to open io.max for %s: %s", name, strerror(errno));
        return -1;
    }

    fprintf(fp, "%s rbps=%lu wbps=%lu\n", device, rbps, wbps);
    fclose(fp);

    log_info("Set I/O limits for %s on %s: read=%lu, write=%lu bps", 
             name, device, rbps, wbps);
    return 0;
}

int cgroup_get_io_stats(const char *name, unsigned long long *read_bytes, unsigned long long *write_bytes) {
    if (!cgroup_exists(name)) {
        log_error("Cgroup %s does not exist", name);
        return -1;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s/io.stat", CGROUP_BASE_PATH, name);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        log_error("Failed to open io.stat for %s: %s", name, strerror(errno));
        return -1;
    }

    char line[512];
    *read_bytes = 0;
    *write_bytes = 0;

    while (fgets(line, sizeof(line), fp)) {
        unsigned long long rbytes, wbytes;
        if (sscanf(line, "%*s rbytes=%llu wbytes=%llu", &rbytes, &wbytes) == 2) {
            *read_bytes += rbytes;
            *write_bytes += wbytes;
        }
    }

    fclose(fp);
    return 0;
}
