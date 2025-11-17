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
}

int main(int argc, char **argv) {
    if (argc < 3) { usage(argv[0]); return 1; }
    if (strcmp(argv[1], "read") == 0) {
        return cgroup_read_metrics(argv[2]);
    } else if (strcmp(argv[1], "create") == 0) {
        return cgroup_create(argv[2]);
    } else if (strcmp(argv[1], "move") == 0) {
        if (argc < 4) { usage(argv[0]); return 1; }
        pid_t pid = (pid_t)atoi(argv[3]);
        return cgroup_move_pid(argv[2], pid);
    } else {
        usage(argv[0]);
        return 1;
    }
}
