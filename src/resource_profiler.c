#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include "../include/resource_profiler.h"

/* Simple resource profiler: reads /proc/<pid>/stat and /proc/<pid>/status to collect
 * utime, stime, vsize, rss and outputs CSV rows.
 * This is a minimal, portable-to-Linux implementation for the assignment.
 */

static int read_proc_stat(pid_t pid, unsigned long *utime, unsigned long *stime, unsigned long *vsize, long *rss) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", (int)pid);
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    /* Fields: pid (1) comm (2) state (3) ... utime (14) stime (15) ... vsize (23) rss (24) */
    int i = 0;
    char buf[4096];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return -1; }
    fclose(f);
    char *p = buf;
    char *token;
    /* iterate tokens carefully because comm can contain spaces inside parentheses */
    /* find the closing ')' of comm */
    token = strchr(p, ')');
    if (!token) return -1;
    p = token + 2; /* skip ") " */
    /* now tokens from state onward, we need fields 3.. so utime is field 14 relative to start */
    /* we'll use sscanf from the substring to pick needed fields */
    /* Build a format that skips appropriately: read up to stime and then vsize rss */
    unsigned long _utime=0, _stime=0, _vsize=0; long _rss=0;
    /* We can sscanf the remainder with many placeholders; this is a pragmatic approach */
    int matched = sscanf(p,
        "%*c %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*u %lu %ld",
        &_utime, &_stime, &_vsize, &_rss);
    if (matched < 4) {
        /* Fallback: try simpler parse scanning tokens */
        char *save = strdup(p);
        char *tok = strtok(save, " ");
        int idx = 3; /* starting from state as field 3 */
        unsigned long fields[30];
        int fi = 0;
        while (tok && fi < 30) {
            fields[fi++] = strtoul(tok, NULL, 10);
            tok = strtok(NULL, " ");
        }
        free(save);
        if (fi >= 24) {
            _utime = fields[11]; /* utime (field 14 overall -> index 11 here) */
            _stime = fields[12];
            _vsize = fields[20];
            _rss = (long)fields[21];
        } else {
            return -1;
        }
    }
    *utime = _utime;
    *stime = _stime;
    *vsize = _vsize;
    *rss = _rss;
    return 0;
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
    fprintf(out, "timestamp_ms,pid,utime,stime,vsize,rss\n");
    for (int i = 0; i < samples; ++i) {
        unsigned long ut=0, st=0, v=0; long r=0;
        if (read_proc_stat(pid, &ut, &st, &v, &r) == 0) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            long long ms = (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
            fprintf(out, "%lld,%d,%lu,%lu,%lu,%ld\n", ms, (int)pid, ut, st, v, r);
            fflush(out);
        } else {
            fprintf(stderr, "rp_run: failed to read /proc for pid %d\n", pid);
            if (outpath) fclose(out);
            return -1;
        }
        if (i + 1 < samples) {
            usleep((useconds_t)interval_ms * 1000);
        }
    }
    if (outpath) fclose(out);
    return 0;
}
