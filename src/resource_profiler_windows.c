#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <psapi.h>
#include "../include/resource_profiler.h"

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "kernel32.lib")

/* Windows implementation of resource profiler using GetProcessTimes and GetProcessMemoryInfo */

typedef struct {
    ULONGLONG user_time;
    ULONGLONG kernel_time;
    ULONGLONG working_set;
    ULONGLONG peak_working_set;
} proc_metrics_t;

static int get_process_metrics(DWORD pid, proc_metrics_t *metrics) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) {
        fprintf(stderr, "Failed to open process %lu\n", pid);
        return -1;
    }

    FILETIME creation_time, exit_time, kernel_time, user_time;
    if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time)) {
        fprintf(stderr, "Failed to get process times\n");
        CloseHandle(hProcess);
        return -1;
    }

    PROCESS_MEMORY_COUNTERS pmc;
    if (!GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        fprintf(stderr, "Failed to get process memory info\n");
        CloseHandle(hProcess);
        return -1;
    }

    /* Convert FILETIME to milliseconds */
    ULONGLONG ut = *((ULONGLONG *)&user_time) / 10000;
    ULONGLONG kt = *((ULONGLONG *)&kernel_time) / 10000;

    metrics->user_time = ut;
    metrics->kernel_time = kt;
    metrics->working_set = pmc.WorkingSetSize;
    metrics->peak_working_set = pmc.PeakWorkingSetSize;

    CloseHandle(hProcess);
    return 0;
}

static double calc_cpu_percent(const proc_metrics_t *prev, const proc_metrics_t *curr, int interval_ms) {
    if (!prev || !curr) return 0.0;
    ULONGLONG proc_delta = (curr->user_time + curr->kernel_time) - (prev->user_time + prev->kernel_time);
    if (interval_ms <= 0) interval_ms = 1;
    double cpu_pct = (proc_delta / (double)interval_ms) * 100.0;
    return (cpu_pct > 100.0) ? 100.0 : cpu_pct;
}

int rp_run(pid_t pid_raw, int interval_ms, int samples, const char *outpath) {
    if (samples <= 0) samples = 1;
    DWORD pid = (DWORD)pid_raw;

    FILE *out = stdout;
    if (outpath) {
        out = fopen(outpath, "w");
        if (!out) {
            perror("fopen");
            return -1;
        }
    }

    fprintf(out, "timestamp_ms,pid,user_time_ms,kernel_time_ms,cpu_percent,working_set_bytes,peak_working_set_bytes\n");

    proc_metrics_t prev = {0}, curr = {0};
    int first = 1;

    for (int i = 0; i < samples; ++i) {
        if (get_process_metrics(pid, &curr) != 0) {
            fprintf(stderr, "Failed to get metrics for pid %lu\n", pid);
            if (outpath) fclose(out);
            return -1;
        }

        SYSTEMTIME st;
        GetSystemTime(&st);
        long long ms = ((long long)st.wHour * 3600000) + ((long long)st.wMinute * 60000) + ((long long)st.wSecond * 1000) + st.wMilliseconds;

        double cpu_pct = 0.0;
        if (!first) {
            cpu_pct = calc_cpu_percent(&prev, &curr, interval_ms);
        }

        fprintf(out, "%lld,%lu,%llu,%llu,%.2f,%llu,%llu\n",
                ms, pid, curr.user_time, curr.kernel_time, cpu_pct, curr.working_set, curr.peak_working_set);
        fflush(out);

        prev = curr;
        first = 0;

        if (i + 1 < samples) {
            Sleep(interval_ms);
        }
    }

    if (outpath) fclose(out);
    return 0;
}
