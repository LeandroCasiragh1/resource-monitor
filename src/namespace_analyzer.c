#include <stdio.h>
#include <sys/types.h>
#include "../include/namespace.h"

int namespace_list_for_pid(pid_t pid) {
    printf("[namespace_analyzer] list namespaces for pid %d\n", pid);
    return 0;
}

int namespace_compare(pid_t pid1, pid_t pid2) {
    printf("[namespace_analyzer] compare %d vs %d\n", pid1, pid2);
    return 0;
}
