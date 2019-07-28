#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define WARN(fmt, ...) //(fprintf(stderr, "W " __FILE__ ": " fmt "\n", ##__VA_ARGS__))
#define INFO(fmt, ...) //(fprintf(stderr, "I " __FILE__ ": " fmt "\n", ##__VA_ARGS__))

#endif
