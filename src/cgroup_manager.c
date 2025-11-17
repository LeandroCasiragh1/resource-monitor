#include <stdio.h>
#include <sys/types.h>
#include "../include/cgroup.h"

int cgroup_read_metrics(const char *cgroup_path) {
    printf("[cgroup_manager] read metrics for %s\n", cgroup_path);
    return 0;
}

int cgroup_create(const char *name) {
    printf("[cgroup_manager] create cgroup %s\n", name);
    return 0;
}

int cgroup_move_pid(const char *cgroup_path, pid_t pid) {
    printf("[cgroup_manager] move pid %d to %s\n", pid, cgroup_path);
    return 0;
}
