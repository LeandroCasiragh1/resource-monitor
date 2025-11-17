#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include "../include/resource_profiler.h"

/* Resource profiler: reads /proc/<pid> to collect CPU, memory, IO and network metrics.
 * Calculates CPU% using /proc/stat and per-interval IO rates using /proc/<pid>/io.
 * Emits CSV by default or JSON if output file ends with .json.
 */

typedef struct {
    unsigned long utime;
    unsigned long stime;
    unsigned long vsize;
    long rss;
    unsigned long minflt;
    unsigned long majflt;
    int threads;
    unsigned long vm_swap_kb;
    unsigned long ctx_voluntary;
    unsigned long ctx_nonvoluntary;
} proc_stat_t;

typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
} cpu_stat_t;

/* Helpers (C, not lambdas) */
static int read_proc_io_fn(pid_t p, unsigned long long *rchar,
                           unsigned long long *wchar,
                           unsigned long long *read_bytes,
                           unsigned long long *write_bytes) {
    char ipath[256];
    snprintf(ipath, sizeof(ipath), "/proc/%d/io", (int)p);
    FILE *iof = fopen(ipath, "r");
    if (!iof) return -1;
    char line[256];
    while (fgets(line, sizeof(line), iof)) {
        (void)sscanf(line, "rchar: %llu", rchar);
        (void)sscanf(line, "wchar: %llu", wchar);
        (void)sscanf(line, "read_bytes: %llu", read_bytes);
        (void)sscanf(line, "write_bytes: %llu", write_bytes);
    }
    fclose(iof);
    return 0;
}

static int count_net_conns_fn(pid_t p, const char *proto) {
    char npath[256];
    snprintf(npath, sizeof(npath), "/proc/%d/net/%s", (int)p, proto);
    FILE *nf = fopen(npath, "r");
    if (!nf) return 0;
    char line[512];
    int lines = 0;
    while (fgets(line, sizeof(line), nf)) lines++;
    fclose(nf);
    return (lines > 0) ? (lines - 1) : 0;
}

/* Read CPU jiffies from /proc/stat (first line) */
static int read_cpu_stat(cpu_stat_t *stat) {
    FILE *f = fopen("/proc/stat", "r");
    if (!f) return -1;
    char buf[256];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return -1; }
    fclose(f);
    /* First line: cpu user nice system idle iowait ... */
    int n = sscanf(buf, "cpu %lu %lu %lu %lu %lu", &stat->user, &stat->nice, &stat->system, &stat->idle, &stat->iowait);
    return (n >= 5) ? 0 : -1;
}

/* Read process stat: utime, stime, vsize, rss */
static int read_proc_stat(pid_t pid, proc_stat_t *stat) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", (int)pid);
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    char buf[4096];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return -1; }
    fclose(f);
    /* Parse after comm ) token into array of fields */
    char *p = strchr(buf, ')');
    if (!p) return -1;
    p += 2; /* skip ") " */
    /* Collect up to 64 numeric fields */
    unsigned long fields[64];
    int fi = 0;
    char *q = p;
    while (*q && fi < 64) {
        char *end;
        unsigned long v = strtoul(q, &end, 10);
        if (end == q) {
            /* skip token (state char etc.) */
            q++;
            continue;
        }
        fields[fi++] = v;
        q = end;
    }
    if (fi < 24) return -1;
    /* Map according to proc(5) after comm/state: indexes offset may vary due to non-numeric state */
    /* To be robust, we re-tokenize using sscanf for mixed tokens */
    /* Minimal set: minflt(8), majflt(10), utime(13), stime(14), vsize(23), rss(24) using 1-based indexing after pid/comm/state */
    /* Use a safer tokenizer: */
    unsigned long minflt=0, majflt=0, utime=0, stime=0, vsize=0; long rss=0;
    int scanned = sscanf(p,
        "%*d %*d %*d %*d %*u %*u %*u %lu %*u %lu %*u %lu %lu %*d %*d %*d %*d %*d %*u %lu %ld",
        &minflt, &majflt, &utime, &stime, &vsize, &rss);
    if (scanned < 6) {
        /* Fallback to array positions (best-effort) */
        minflt = fields[7];
        majflt = fields[9];
        utime = fields[12];
        stime = fields[13];
        vsize = fields[22];
        rss = (long)fields[23];
    }
    stat->minflt = minflt;
    stat->majflt = majflt;
    stat->utime = utime;
    stat->stime = stime;
    stat->vsize = vsize;
    stat->rss = rss;
    /* Read extra info from /proc/<pid>/status */
    char spath[256];
    snprintf(spath, sizeof(spath), "/proc/%d/status", (int)pid);
    FILE *sf = fopen(spath, "r");
    if (sf) {
        char line[256];
        while (fgets(line, sizeof(line), sf)) {
            if (sscanf(line, "Threads: %d", &stat->threads) == 1) continue;
            if (sscanf(line, "voluntary_ctxt_switches: %lu", &stat->ctx_voluntary) == 1) continue;
            if (sscanf(line, "nonvoluntary_ctxt_switches: %lu", &stat->ctx_nonvoluntary) == 1) continue;
            if (sscanf(line, "VmSwap: %lu", &stat->vm_swap_kb) == 1) continue;
        }
        fclose(sf);
    }
    return 0;
}

/* Calculate CPU percentage: (delta_utime + delta_stime) / (delta_cpu_total) * 100 */
static double calc_cpu_percent(const proc_stat_t *prev, const proc_stat_t *curr,
                               const cpu_stat_t *prev_cpu, const cpu_stat_t *curr_cpu) {
    if (!prev || !curr || !prev_cpu || !curr_cpu) return 0.0;
    
    unsigned long proc_delta_ticks = (curr->utime + curr->stime) - (prev->utime + prev->stime);
    unsigned long cpu_total_prev = prev_cpu->user + prev_cpu->nice + prev_cpu->system + prev_cpu->idle + prev_cpu->iowait;
    unsigned long cpu_total_curr = curr_cpu->user + curr_cpu->nice + curr_cpu->system + curr_cpu->idle + curr_cpu->iowait;
    unsigned long cpu_delta_ticks = cpu_total_curr - cpu_total_prev;
    
    if (cpu_delta_ticks == 0) return 0.0;
    
    double cpu_percent = (proc_delta_ticks / (double)cpu_delta_ticks) * 100.0;
    return (cpu_percent > 100.0) ? 100.0 : cpu_percent;
}

int rp_run(pid_t pid, int interval_ms, int samples, const char *outpath) {
    if (samples <= 0) samples = 1;
    FILE *out = stdout;
    if (outpath) {
        out = fopen(outpath, "w");
        if (!out) {
            perror("fopen");
            return -1;
        }
    }
    int emit_json = 0;
    if (outpath) {
        const char *dot = strrchr(outpath, '.');
        if (dot && strcmp(dot, ".json") == 0) emit_json = 1;
    }
    /* Headers */
    if (emit_json) {
        fprintf(out, "[\n");
    } else {
        fprintf(out, "timestamp_ms,pid,utime_ticks,stime_ticks,cpu_percent,vsize_bytes,rss_pages,threads,minflt,majflt,vm_swap_kb,ctx_voluntary,ctx_nonvoluntary,io_rchar,io_wchar,io_read_bytes,io_write_bytes,io_read_bps,io_write_bps,net_tcp_conns,net_udp_conns\n");
    }
    
    proc_stat_t prev_proc = {0}, curr_proc = {0};
    cpu_stat_t prev_cpu = {0}, curr_cpu = {0};
    /* IO cumulative values for deltas */
    unsigned long long prev_read_bytes=0, prev_write_bytes=0;
    int first = 1;
    /* Small helpers moved to C static functions above */
    
    for (int i = 0; i < samples; ++i) {
        /* Read current system and process stats */
        if (read_cpu_stat(&curr_cpu) != 0) {
            fprintf(stderr, "rp_run: failed to read /proc/stat\n");
            if (outpath) fclose(out);
            return -1;
        }
        if (read_proc_stat(pid, &curr_proc) != 0) {
            fprintf(stderr, "rp_run: failed to read /proc for pid %d\n", pid);
            if (outpath) fclose(out);
            return -1;
        }
        
        /* Get timestamp */
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        long long ms = (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
        
        /* Calculate CPU% (skip on first sample since no delta) */
        double cpu_pct = 0.0;
        if (!first) {
            cpu_pct = calc_cpu_percent(&prev_proc, &curr_proc, &prev_cpu, &curr_cpu);
        }
        
        /* IO counters */
        unsigned long long rchar=0, wchar=0, read_bytes=0, write_bytes=0;
        (void)read_proc_io_fn(pid, &rchar, &wchar, &read_bytes, &write_bytes);
        double interval_s = (double)interval_ms / 1000.0;
        double read_bps = (!first && interval_s > 0) ? (read_bytes - prev_read_bytes) / interval_s : 0.0;
        double write_bps = (!first && interval_s > 0) ? (write_bytes - prev_write_bytes) / interval_s : 0.0;
        prev_read_bytes = read_bytes; prev_write_bytes = write_bytes;

        /* Net connections */
        int tcp_conns = count_net_conns_fn(pid, "tcp") + count_net_conns_fn(pid, "tcp6");
        int udp_conns = count_net_conns_fn(pid, "udp") + count_net_conns_fn(pid, "udp6");

        if (emit_json) {
            fprintf(out,
            "  {\"timestamp_ms\": %lld, \"pid\": %d, \"utime_ticks\": %lu, \"stime_ticks\": %lu, \"cpu_percent\": %.2f, \"vsize_bytes\": %lu, \"rss_pages\": %ld, \"threads\": %d, \"minflt\": %lu, \"majflt\": %lu, \"vm_swap_kb\": %lu, \"ctx_voluntary\": %lu, \"ctx_nonvoluntary\": %lu, \"io_rchar\": %llu, \"io_wchar\": %llu, \"io_read_bytes\": %llu, \"io_write_bytes\": %llu, \"io_read_bps\": %.2f, \"io_write_bps\": %.2f, \"net_tcp_conns\": %d, \"net_udp_conns\": %d }%s\n",
            ms, (int)pid, curr_proc.utime, curr_proc.stime, cpu_pct, curr_proc.vsize, curr_proc.rss,
            curr_proc.threads, curr_proc.minflt, curr_proc.majflt, curr_proc.vm_swap_kb,
            curr_proc.ctx_voluntary, curr_proc.ctx_nonvoluntary,
            rchar, wchar, read_bytes, write_bytes, read_bps, write_bps, tcp_conns, udp_conns,
            (i + 1 < samples) ? "," : "");
        } else {
            fprintf(out, "%lld,%d,%lu,%lu,%.2f,%lu,%ld,%d,%lu,%lu,%lu,%lu,%lu,%llu,%llu,%llu,%llu,%.0f,%.0f,%d,%d\n",
            ms, (int)pid, curr_proc.utime, curr_proc.stime, cpu_pct, curr_proc.vsize, curr_proc.rss,
            curr_proc.threads, curr_proc.minflt, curr_proc.majflt, curr_proc.vm_swap_kb,
            curr_proc.ctx_voluntary, curr_proc.ctx_nonvoluntary,
            rchar, wchar, read_bytes, write_bytes, read_bps, write_bps, tcp_conns, udp_conns);
        }
        fflush(out);
        
        /* Save for next iteration */
        prev_proc = curr_proc;
        prev_cpu = curr_cpu;
        first = 0;
        
        if (i + 1 < samples) {
            usleep((useconds_t)interval_ms * 1000);
        }
    }
    if (emit_json) fprintf(out, "]\n");
    if (outpath) fclose(out);
    return 0;
}
