#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/monitor.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }
    pid_t pid = (pid_t)atoi(argv[1]);
    monitor_init();
    monitor_watch_pid(pid, 1000);
    monitor_stop();
    return 0;
}
