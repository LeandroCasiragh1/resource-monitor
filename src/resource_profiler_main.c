#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/resource_profiler.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pid> [interval_ms] [samples] [out.csv]\n", argv[0]);
        return 1;
    }
    pid_t pid = (pid_t)atoi(argv[1]);
    int interval = 1000;
    int samples = 1;
    const char *out = NULL;
    if (argc >= 3) interval = atoi(argv[2]);
    if (argc >= 4) samples = atoi(argv[3]);
    if (argc >= 5) out = argv[4];
    return rp_run(pid, interval, samples, out);
}
