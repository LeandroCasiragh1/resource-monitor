#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include "../include/resource_profiler.h"

/* Simple resource profiler: reads /proc/<pid>/stat and calculates CPU% using /proc/stat.
 * Outputs CSV with timestamp, pid, utime, stime, cpu_percent, vsize, rss.
 * This is a minimal, portable-to-Linux implementation for the assignment.
 */

typedef struct {
    unsigned long utime;
    unsigned long stime;
    unsigned long vsize;
    long rss;
} proc_stat_t;

typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
} cpu_stat_t;

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
    
    /* Parse: find closing ) of comm, then extract fields */
    char *p = buf;
    char *token = strchr(p, ')');
    if (!token) return -1;
    p = token + 2; /* skip ") " */
    
    unsigned long _utime=0, _stime=0, _vsize=0; long _rss=0;
    int matched = sscanf(p,
        "%*c %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*u %lu %ld",
        &_utime, &_stime, &_vsize, &_rss);
    
    if (matched < 4) {
        /* Fallback: token-by-token parsing */
        char *save = strdup(p);
        if (!save) return -1;
        char *tok = strtok(save, " ");
        unsigned long fields[30];
        int fi = 0;
        while (tok && fi < 30) {
            fields[fi++] = strtoul(tok, NULL, 10);
            tok = strtok(NULL, " ");
        }
        free(save);
        if (fi >= 24) {
            _utime = fields[11];  /* utime relative index in parsed fields */
            _stime = fields[12];  /* stime */
            _vsize = fields[20];  /* vsize */
            _rss = (long)fields[21]; /* rss */
        } else {
            return -1;
        }
    }
    stat->utime = _utime;
    stat->stime = _stime;
    stat->vsize = _vsize;
    stat->rss = _rss;
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
    
    /* CSV header */
    fprintf(out, "timestamp_ms,pid,utime_ticks,stime_ticks,cpu_percent,vsize_bytes,rss_pages\n");
    
    proc_stat_t prev_proc = {0}, curr_proc = {0};
    cpu_stat_t prev_cpu = {0}, curr_cpu = {0};
    int first = 1;
    
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
        
        /* Write CSV row */
        fprintf(out, "%lld,%d,%lu,%lu,%.2f,%lu,%ld\n",
                ms, (int)pid, curr_proc.utime, curr_proc.stime, cpu_pct, curr_proc.vsize, curr_proc.rss);
        fflush(out);
        
        /* Save for next iteration */
        prev_proc = curr_proc;
        prev_cpu = curr_cpu;
        first = 0;
        
        if (i + 1 < samples) {
            usleep((useconds_t)interval_ms * 1000);
        }
    }
    
    if (outpath) fclose(out);
    return 0;
}
