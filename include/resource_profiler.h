#ifndef RESOURCE_PROFILER_H
#define RESOURCE_PROFILER_H

#include <sys/types.h>

/* Run resource profiler: collect samples for pid with given interval (ms) and save to outpath.
 * If outpath ends with .json, emits JSON; otherwise CSV.
 * CSV includes: timestamp_ms,pid,utime_ticks,stime_ticks,cpu_percent,vsize_bytes,rss_pages,threads,
 *               minflt,majflt,vm_swap_kb,ctx_voluntary,ctx_nonvoluntary,
 *               io_rchar,io_wchar,io_read_bytes,io_write_bytes,io_read_bps,io_write_bps,
 *               net_tcp_conns,net_udp_conns
 * Returns 0 on success, non-zero on error.
 */
int rp_run(pid_t pid, int interval_ms, int samples, const char *outpath);

#endif // RESOURCE_PROFILER_H
