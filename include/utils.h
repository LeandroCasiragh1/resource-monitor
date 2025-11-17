#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// File utilities
FILE* safe_fopen(const char *path, const char *mode);
int ensure_directory(const char *path);
char* read_file_content(const char *path);

// String utilities
void trim_string(char *str);
int parse_line_value(const char *line, const char *key, unsigned long *value);
int parse_line_value_ull(const char *line, const char *key, unsigned long long *value);

// Time utilities
void get_timestamp(char *buffer, size_t size);
double get_elapsed_time(struct timespec *start, struct timespec *end);

// Math utilities
double calculate_percentage(unsigned long used, unsigned long total);
double calculate_rate(unsigned long long current, unsigned long long previous, double time_diff);

// Logging utilities
void log_message(const char *level, const char *format, ...);
void log_error(const char *format, ...);
void log_info(const char *format, ...);

#endif // UTILS_H
