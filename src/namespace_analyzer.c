#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <sched.h>
#include <signal.h>
#include "../include/namespace.h"

static const char *ns_types[] = {"mnt","pid","net","ipc","uts","user","cgroup", NULL};

static int read_ns_link(pid_t pid, const char *ns_type, char *out, size_t outlen) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/ns/%s", (int)pid, ns_type);
    ssize_t r = readlink(path, out, outlen - 1);
    if (r < 0) return -1;
    out[r] = '\0';
    return 0;
}

int namespace_list_for_pid(pid_t pid) {
    char buf[128];
    printf("Namespaces for pid %d:\n", (int)pid);
    for (const char **p = ns_types; *p; ++p) {
        if (read_ns_link(pid, *p, buf, sizeof(buf)) == 0) {
            printf("  %s: %s\n", *p, buf);
        } else {
            printf("  %s: (unavailable)\n", *p);
        }
    }
    return 0;
}

int namespace_compare(pid_t pid1, pid_t pid2) {
    char a[128], b[128];
    int identical = 1;
    printf("Comparing namespaces: %d vs %d\n", (int)pid1, (int)pid2);
    for (const char **p = ns_types; *p; ++p) {
        if (read_ns_link(pid1, *p, a, sizeof(a)) != 0) strcpy(a, "(unavailable)");
        if (read_ns_link(pid2, *p, b, sizeof(b)) != 0) strcpy(b, "(unavailable)");
        if (strcmp(a, b) == 0) {
            printf("  %s: same (%s)\n", *p, a);
        } else {
            printf("  %s: DIFFER (%s) vs (%s)\n", *p, a, b);
            identical = 0;
        }
    }
    return identical ? 0 : 1;
}

/* Map processes by a given namespace type. We'll collect (nsid, pid) pairs, sort by nsid and print grouped. */
int namespace_map_by_type(const char *ns_type) {
    DIR *d = opendir("/proc");
    if (!d) {
        perror("opendir /proc");
        return -1;
    }
    struct dirent *de;
    typedef struct { char nsid[128]; int pid; } pair_t;
    size_t cap = 4096;
    size_t n = 0;
    pair_t *arr = calloc(cap, sizeof(pair_t));
    if (!arr) { closedir(d); return -1; }
    while ((de = readdir(d)) != NULL) {
        if (de->d_type != DT_DIR) continue;
        char *endptr;
        long pid = strtol(de->d_name, &endptr, 10);
        if (*endptr != '\0') continue; /* not a PID */
        char buf[128];
        if (read_ns_link((pid_t)pid, ns_type, buf, sizeof(buf)) != 0) continue;
        if (n >= cap) {
            cap *= 2;
            pair_t *tmp = realloc(arr, cap * sizeof(pair_t));
            if (!tmp) break;
            arr = tmp;
        }
        strncpy(arr[n].nsid, buf, sizeof(arr[n].nsid)-1);
        arr[n].nsid[sizeof(arr[n].nsid)-1] = '\0';
        arr[n].pid = (int)pid;
        n++;
    }
    closedir(d);
    if (n == 0) {
        free(arr);
        printf("No processes found or no namespace entries for type '%s'\n", ns_type);
        return 0;
    }
    /* sort by nsid */
    int cmp(const void *A, const void *B) {
        const pair_t *a = A; const pair_t *b = B;
        int r = strcmp(a->nsid, b->nsid);
        if (r != 0) return r;
        return a->pid - b->pid;
    }
    qsort(arr, n, sizeof(pair_t), cmp);
    /* print grouped */
    char cur[128] = "";
    for (size_t i = 0; i < n; ++i) {
        if (strcmp(cur, arr[i].nsid) != 0) {
            if (cur[0] != '\0') printf("\n");
            strcpy(cur, arr[i].nsid);
            printf("%s: %d", cur, arr[i].pid);
        } else {
            printf(" %d", arr[i].pid);
        }
    }
    printf("\n");
    free(arr);
    return 0;
}

/* Measure overhead to create a new namespace of a given type (iterations averaged) */
int namespace_creation_overhead(const char *ns_type, int iterations) {
    if (iterations <= 0) iterations = 10;
    int flag = 0;
    if (strcmp(ns_type, "pid") == 0) flag = CLONE_NEWPID;
    else if (strcmp(ns_type, "net") == 0) flag = CLONE_NEWNET;
    else if (strcmp(ns_type, "mnt") == 0 || strcmp(ns_type, "mount") == 0) flag = CLONE_NEWNS;
    else if (strcmp(ns_type, "uts") == 0) flag = CLONE_NEWUTS;
    else if (strcmp(ns_type, "ipc") == 0) flag = CLONE_NEWIPC;
    else if (strcmp(ns_type, "user") == 0) flag = CLONE_NEWUSER;
    else if (strcmp(ns_type, "cgroup") == 0) flag = 0; /* v2 not namespaced via clone */
    else { fprintf(stderr, "Unknown ns_type: %s\n", ns_type); return 1; }

    /* Simple function to run in child */
    static int child(void *arg) { (void)arg; _exit(0); }
    const size_t STACK_SIZE = 1024*1024;
    char *stack = malloc(STACK_SIZE);
    if (!stack) { perror("malloc"); return 1; }
    char *top = stack + STACK_SIZE;

    struct timespec t0, t1;
    double total_ms = 0.0; int ok = 0;
    for (int i = 0; i < iterations; ++i) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        pid_t pid = clone(child, top, flag | SIGCHLD, NULL);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        if (pid < 0) {
            perror("clone");
            fprintf(stderr, "Hint: some namespaces require root (EPERM)\n");
            free(stack);
            return 1;
        }
        int status; (void)waitpid(pid, &status, 0);
        double ms = (t1.tv_sec - t0.tv_sec)*1000.0 + (t1.tv_nsec - t0.tv_nsec)/1e6;
        total_ms += ms; ok++;
    }
    free(stack);
    if (ok == 0) { fprintf(stderr, "No successful iterations\n"); return 1; }
    printf("Namespace creation overhead (%s): %.3f ms (avg over %d)\n", ns_type, total_ms/ok, ok);
    return 0;
}

/* Global report: counts processes per namespace type */
int namespace_system_report(void) {
    const char *types[] = {"mnt","pid","net","ipc","uts","user","cgroup",NULL};
    printf("Namespace System Report\n");
    for (const char **p = types; *p; ++p) {
        /* Reuse map to accumulate counts */
        DIR *d = opendir("/proc"); if (!d) { perror("opendir /proc"); return -1; }
        struct dirent *de; int count = 0; char last[128] = ""; int groups = 0;
        while ((de = readdir(d)) != NULL) {
            if (de->d_type != DT_DIR) continue;
            char *endptr; long pid = strtol(de->d_name, &endptr, 10);
            if (*endptr != '\0') continue;
            char buf[128];
            if (read_ns_link((pid_t)pid, *p, buf, sizeof(buf)) != 0) continue;
            count++;
            if (strcmp(buf, last) != 0) { strcpy(last, buf); groups++; }
        }
        closedir(d);
        printf("  %-6s: %d processes across ~%d namespaces\n", *p, count, groups);
    }
    return 0;
}

