#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
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

