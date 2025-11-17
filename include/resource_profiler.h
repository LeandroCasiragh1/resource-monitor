#ifndef RESOURCE_PROFILER_H
#define RESOURCE_PROFILER_H

#include <sys/types.h>

int rp_run(pid_t pid, int interval_ms, int samples, const char *outpath);

#endif // RESOURCE_PROFILER_H
