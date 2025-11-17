#include "../include/utils.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

FILE* safe_fopen(const char *path, const char *mode) {
    FILE *fp = fopen(path, mode);
    if (!fp) {
        log_error("Failed to open file %s: %s", path, strerror(errno));
    }
    return fp;
}

int ensure_directory(const char *path) {
    struct stat st = {0};
    
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0755) == -1) {
            log_error("Failed to create directory %s: %s", path, strerror(errno));
            return -1;
        }
    }
    return 0;
}

char* read_file_content(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *content = malloc(size + 1);
    if (!content) {
        fclose(fp);
        return NULL;
    }

    fread(content, 1, size, fp);
    content[size] = '\0';
    fclose(fp);

    return content;
}

void trim_string(char *str) {
    if (!str) return;

    char *start = str;
    while (isspace(*start)) start++;

    if (*start == '\0') {
        *str = '\0';
        return;
    }

    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) end--;
    *(end + 1) = '\0';

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

int parse_line_value(const char *line, const char *key, unsigned long *value) {
    if (strstr(line, key) == line) {
        const char *val_str = line + strlen(key);
        while (isspace(*val_str)) val_str++;
        *value = strtoul(val_str, NULL, 10);
        return 1;
    }
    return 0;
}

int parse_line_value_ull(const char *line, const char *key, unsigned long long *value) {
    if (strstr(line, key) == line) {
        const char *val_str = line + strlen(key);
        while (isspace(*val_str)) val_str++;
        *value = strtoull(val_str, NULL, 10);
        return 1;
    }
    return 0;
}

void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

double get_elapsed_time(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) + 
           (end->tv_nsec - start->tv_nsec) / 1000000000.0;
}

double calculate_percentage(unsigned long used, unsigned long total) {
    if (total == 0) return 0.0;
    return (100.0 * used) / total;
}

double calculate_rate(unsigned long long current, unsigned long long previous, double time_diff) {
    if (time_diff <= 0.0) return 0.0;
    return (current - previous) / time_diff;
}

void log_message(const char *level, const char *format, ...) {
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    
    printf("[%s] [%s] ", timestamp, level);
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
    fflush(stdout);
}

void log_error(const char *format, ...) {
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    
    fprintf(stderr, "[%s] [ERROR] ", timestamp);
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
    fflush(stderr);
}

void log_info(const char *format, ...) {
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    
    printf("[%s] [INFO] ", timestamp);
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
    fflush(stdout);
}
