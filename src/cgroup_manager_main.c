#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/cgroup.h"

static void usage(const char *p) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s read <cgroup_path>\n", p);
    fprintf(stderr, "  %s create <name>\n", p);
    fprintf(stderr, "  %s move <cgroup_path> <pid>\n", p);
    fprintf(stderr, "  %s set-cpu <name> <quota> <period>\n", p);
    fprintf(stderr, "  %s set-mem <name> <bytes>\n", p);
}

int main(int argc, char **argv) {
    if (argc < 2) { usage(argv[0]); return 1; }
    if (strcmp(argv[1], "read") == 0) {
        if (argc < 3) { usage(argv[0]); return 1; }
        return cgroup_read_metrics(argv[2]);
    } else if (strcmp(argv[1], "create") == 0) {
        if (argc < 3) { usage(argv[0]); return 1; }
        return cgroup_create(argv[2]);
    } else if (strcmp(argv[1], "move") == 0) {
        if (argc < 4) { usage(argv[0]); return 1; }
        pid_t pid = (pid_t)atoi(argv[3]);
        return cgroup_move_pid(argv[2], pid);
    } else if (strcmp(argv[1], "set-cpu") == 0) {
        if (argc < 5) { usage(argv[0]); return 1; }
        long quota = atol(argv[3]);
        long period = atol(argv[4]);
        return cgroup_set_cpu_limit_quota(argv[2], quota, period);
    } else if (strcmp(argv[1], "set-mem") == 0) {
        if (argc < 4) { usage(argv[0]); return 1; }
        unsigned long bytes = strtoul(argv[3], NULL, 10);
        return cgroup_set_memory_max(argv[2], bytes);
    } else {
        usage(argv[0]);
        return 1;
    }
}
