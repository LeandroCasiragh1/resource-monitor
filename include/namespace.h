#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <sys/types.h>

int namespace_list_for_pid(pid_t pid);
int namespace_compare(pid_t pid1, pid_t pid2);

#endif // NAMESPACE_H
