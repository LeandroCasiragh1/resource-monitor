#ifndef RESOURCE_PROFILER_H
#define RESOURCE_PROFILER_H

#include <sys/types.h>

/* Run resource profiler: collect samples for pid with given interval (ms) and save to outpath.
 * CSV includes: timestamp_ms, pid, utime_ticks, stime_ticks, cpu_percent, vsize, rss
 * Returns 0 on success, non-zero on error.
 */
int rp_run(pid_t pid, int interval_ms, int samples, const char *outpath);

#endif // RESOURCE_PROFILER_H
