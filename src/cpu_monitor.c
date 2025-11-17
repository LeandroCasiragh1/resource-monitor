#include <stdio.h>
#include "../include/monitor.h"

/* Placeholder implementation for CPU monitor
 * Real implementation should read /proc/<pid>/stat and calculate deltas.
 */
void monitor_init(void) {
    /* init resources */
}

void monitor_watch_pid(pid_t pid, int interval_ms) {
    printf("[cpu_monitor] watching pid %d every %d ms\n", pid, interval_ms);
}

void monitor_stop(void) {
    /* cleanup */
}
