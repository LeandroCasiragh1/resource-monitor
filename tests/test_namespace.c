#include <stdio.h>
#include <stdlib.h>
#include "../include/namespace.h"

int main(void) {
    /* Basic smoke test: list namespaces for PID 1 (on Linux should usually succeed) */
    int r = namespace_list_for_pid(1);
    if (r != 0) {
        printf("test_namespace: namespace_list_for_pid(1) returned %d (may not be Linux)\n", r);
        return 1;
    }
    printf("test_namespace: OK (listed namespaces for PID 1)\n");
    return 0;
}
