#ifndef CGROUP_V2_H
#define CGROUP_V2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CGROUP_BASE_PATH "/sys/fs/cgroup"
#define MAX_PATH_LEN 512

// Cgroup structure
typedef struct {
    char name[256];
    char path[MAX_PATH_LEN];
    pid_t pid;
} CgroupInfo;

// Cgroup creation and management
int cgroup_create(const char *name);
int cgroup_delete(const char *name);
int cgroup_add_process(const char *name, pid_t pid);
int cgroup_remove_process(const char *name, pid_t pid);

// CPU controller
int cgroup_set_cpu_max(const char *name, long quota, long period);
int cgroup_get_cpu_usage(const char *name, unsigned long long *usage);
int cgroup_set_cpu_weight(const char *name, int weight);

// Memory controller
int cgroup_set_memory_max(const char *name, unsigned long bytes);
int cgroup_get_memory_usage(const char *name, unsigned long *usage);
int cgroup_set_memory_high(const char *name, unsigned long bytes);

// I/O controller
int cgroup_set_io_max(const char *name, const char *device, unsigned long rbps, unsigned long wbps);
int cgroup_get_io_stats(const char *name, unsigned long long *read_bytes, unsigned long long *write_bytes);

// Utility functions
int cgroup_exists(const char *name);
int cgroup_is_v2_available(void);
char* cgroup_get_full_path(const char *name);

#endif // CGROUP_V2_H
