#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <sys/types.h>

/* List all namespaces for a given PID (prints to stdout). Returns 0 on success. */
int namespace_list_for_pid(pid_t pid);

/* Compare namespaces between two PIDs and print differences. Returns 0 if identical, 1 otherwise. */
int namespace_compare(pid_t pid1, pid_t pid2);

/* Map processes by namespace type (e.g., "pid", "net", "mnt", "uts", "ipc", "user", "cgroup").
 * Prints lines grouped by namespace id: NSID: pid1 pid2 ...
 */
int namespace_map_by_type(const char *ns_type);

/* Measure average time to create a given namespace type over N iterations */
int namespace_creation_overhead(const char *ns_type, int iterations);

/* Print a global system report with per-namespace counts */
int namespace_system_report(void);

#endif // NAMESPACE_H
