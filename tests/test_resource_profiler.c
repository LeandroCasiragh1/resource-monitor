#include <stdio.h>
#include <stdlib.h>
#include "../include/resource_profiler.h"

int main(void) {
    /* Test: calling rp_run on a non-existent PID should fail (return non-zero) */
    pid_t fake = 999999; /* likely not present on test systems */
    int r = rp_run(fake, 100, 1, NULL);
    if (r == 0) {
        printf("test_resource_profiler: expected failure for nonexistent PID, but got success\n");
        return 1;
    }
    printf("test_resource_profiler: OK (rp_run returned %d for pid %d)\n", r, (int)fake);
    return 0;
}
