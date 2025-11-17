#ifndef CGROUP_H
#define CGROUP_H

#include <sys/types.h>

int cgroup_read_metrics(const char *cgroup_path);
int cgroup_create(const char *name);
int cgroup_move_pid(const char *cgroup_path, pid_t pid);
int cgroup_set_cpu_limit_quota(const char *name, long quota, long period);
int cgroup_set_memory_max(const char *name, unsigned long bytes);

#endif // CGROUP_H
