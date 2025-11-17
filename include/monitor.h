#ifndef MONITOR_H
#define MONITOR_H

#include <stdint.h>
#include <sys/types.h>

void monitor_init(void);
void monitor_watch_pid(pid_t pid, int interval_ms);
void monitor_stop(void);

#endif // MONITOR_H
