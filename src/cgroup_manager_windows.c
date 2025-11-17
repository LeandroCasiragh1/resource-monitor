#include <stdio.h>
#include <windows.h>

/* Placeholder cgroup manager for Windows - cgroups are Linux-specific */

int cgroup_read_metrics(const char *cgroup_path) {
    printf("cgroup_read_metrics: Cgroups are Linux-specific. Not available on Windows.\n");
    printf("For Linux testing, use a WSL2 environment or native Linux VM.\n");
    return -1;
}

int cgroup_create(const char *name) {
    printf("cgroup_create: Cgroups are Linux-specific. Not available on Windows.\n");
    return -1;
}

int cgroup_move_pid(const char *cgroup_path, pid_t pid) {
    printf("cgroup_move_pid: Cgroups are Linux-specific. Not available on Windows.\n");
    return -1;
}
