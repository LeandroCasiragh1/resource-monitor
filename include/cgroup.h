#ifndef CGROUP_H
#define CGROUP_H

#include <sys/types.h>

int cgroup_read_metrics(const char *cgroup_path);
int cgroup_create(const char *name);
int cgroup_move_pid(const char *cgroup_path, pid_t pid);

#endif // CGROUP_H
