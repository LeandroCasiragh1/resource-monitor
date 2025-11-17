#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/namespace.h"

static void usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s list <pid>\n", prog);
    fprintf(stderr, "  %s compare <pid1> <pid2>\n", prog);
    fprintf(stderr, "  %s map <ns_type>\n", prog);
    fprintf(stderr, "  %s overhead <ns_type> [iterations]\n", prog);
    fprintf(stderr, "  %s report\n", prog);
}

int main(int argc, char **argv) {
    if (argc < 2) { usage(argv[0]); return 1; }
    if (strcmp(argv[1], "list") == 0) {
        if (argc < 3) { usage(argv[0]); return 1; }
        pid_t pid = (pid_t)atoi(argv[2]);
        return namespace_list_for_pid(pid);
    } else if (strcmp(argv[1], "compare") == 0) {
        if (argc < 4) { usage(argv[0]); return 1; }
        pid_t a = (pid_t)atoi(argv[2]);
        pid_t b = (pid_t)atoi(argv[3]);
        return namespace_compare(a,b);
    } else if (strcmp(argv[1], "map") == 0) {
        if (argc < 3) { usage(argv[0]); return 1; }
        const char *nt = argv[2];
        return namespace_map_by_type(nt);
    } else if (strcmp(argv[1], "overhead") == 0) {
        if (argc < 3) { usage(argv[0]); return 1; }
        const char *nt = argv[2];
        int it = (argc >= 4) ? atoi(argv[3]) : 10;
        return namespace_creation_overhead(nt, it);
    } else if (strcmp(argv[1], "report") == 0) {
        return namespace_system_report();
    } else {
        usage(argv[0]);
        return 1;
    }
}
