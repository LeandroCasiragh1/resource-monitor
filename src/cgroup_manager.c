
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include "../include/cgroup.h"

/* Minimal helper: join base cgroup path for v2 (/sys/fs/cgroup) or accept full path */
static const char *CGROUP_ROOT = "/sys/fs/cgroup";

static int write_file(const char *path, const char *data) {
    int fd = open(path, O_WRONLY | O_CLOEXEC);
    if (fd < 0) return -1;
    ssize_t w = write(fd, data, strlen(data));
    close(fd);
    return (w == (ssize_t)strlen(data)) ? 0 : -1;
}

int cgroup_read_metrics(const char *cgroup_path) {
    /* Try to read some common cgroup v2 files: cpu.stat, memory.current, io.stat, cgroup.stat */
    char path[512];
    int any = 0;
    struct { const char *file; const char *label; } probes[] = {
        {"cgroup.stat", "cgroup.stat"},
        {"cpu.stat", "cpu.stat"},
        {"memory.current", "memory.current"},
        {"memory.stat", "memory.stat"},
        {"io.stat", "io.stat"},
        {"cgroup.controllers", "controllers"},
        {NULL, NULL}
    };

    for (int i = 0; probes[i].file; ++i) {
        snprintf(path, sizeof(path), "%s/%s", cgroup_path, probes[i].file);
        FILE *f = fopen(path, "r");
        if (!f) continue;
        printf("--- %s (%s) ---\n", probes[i].label, path);
        char buf[1024];
        while (fgets(buf, sizeof(buf), f)) fputs(buf, stdout);
        fclose(f);
        any = 1;
    }
    if (!any) {
        /* try with root prefix */
        for (int i = 0; probes[i].file; ++i) {
            snprintf(path, sizeof(path), "%s/%s/%s", CGROUP_ROOT, cgroup_path, probes[i].file);
            FILE *f = fopen(path, "r");
            if (!f) continue;
            printf("--- %s (%s) ---\n", probes[i].label, path);
            char buf[1024];
            while (fgets(buf, sizeof(buf), f)) fputs(buf, stdout);
            fclose(f);
            any = 1;
        }
    }
    if (!any) {
        fprintf(stderr, "No cgroup metrics found under '%s' or '%s/%s'\n", cgroup_path, CGROUP_ROOT, cgroup_path);
        return -1;
    }
    return 0;
}

int cgroup_create(const char *name) {
    /* Create a new subtree in cgroup v2 by making a directory under /sys/fs/cgroup/<name> */
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", CGROUP_ROOT, name);
    if (mkdir(path, 0755) == 0) {
        printf("Created cgroup: %s\n", path);
        return 0;
    }
    if (errno == EEXIST) {
        fprintf(stderr, "Cgroup already exists: %s\n", path);
        return 1;
    }
    perror("mkdir");
    return -1;
}

int cgroup_move_pid(const char *cgroup_path, pid_t pid) {
    /* Move a PID into a cgroup by writing to cgroup.procs (v2) or tasks (v1) */
    char path[512];
    int ret = -1;
    /* try direct path */
    snprintf(path, sizeof(path), "%s/%s", cgroup_path, "cgroup.procs");
    if (access(path, W_OK) == 0) {
        char tmp[64];
        snprintf(tmp, sizeof(tmp), "%d", (int)pid);
        ret = write_file(path, tmp);
        if (ret == 0) {
            printf("Moved pid %d to %s (via cgroup.procs)\n", (int)pid, cgroup_path);
            return 0;
        }
    }
    /* try with root prefix */
    snprintf(path, sizeof(path), "%s/%s/%s", CGROUP_ROOT, cgroup_path, "cgroup.procs");
    if (access(path, W_OK) == 0) {
        char tmp[64];
        snprintf(tmp, sizeof(tmp), "%d", (int)pid);
        ret = write_file(path, tmp);
        if (ret == 0) {
            printf("Moved pid %d to %s (via %s)\n", (int)pid, cgroup_path, path);
            return 0;
        }
    }
    /* fallback: try tasks file (cgroups v1) */
    snprintf(path, sizeof(path), "%s/%s/%s", CGROUP_ROOT, cgroup_path, "tasks");
    if (access(path, W_OK) == 0) {
        char tmp[64]; snprintf(tmp, sizeof(tmp), "%d", (int)pid);
        ret = write_file(path, tmp);
        if (ret == 0) { printf("Moved pid %d to %s (via tasks)\n", (int)pid, path); return 0; }
    }
    fprintf(stderr, "Failed to move pid %d into cgroup %s (no writable cgroup.procs/tasks found)\n", (int)pid, cgroup_path);
    return -1;
}
