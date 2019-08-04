#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define WARN(fmt, ...) //(log_write_format("W " __FILE__ ": " fmt "\n", ##__VA_ARGS__))
#define INFO(fmt, ...) //(log_write_format("I " __FILE__ ": " fmt "\n", ##__VA_ARGS__))

extern void log_init(void);
extern void log_write(const char * buf, size_t len);
//extern void log_write_format(const char * fmt, ...);

#endif
