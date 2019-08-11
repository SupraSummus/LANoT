#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#include "FreeRTOSConfig.h"

// log is listening on fd 2 (stderr) and using fd 3 as backend
#define LOG_BACKEND_FD (3)
#define LOG_TASK_PRIORITY (SYSTEM_TASK_PRIORITY)

#define WARN(fmt, ...) (fprintf(stderr, "W " __FILE__ ": " fmt "\n", ##__VA_ARGS__))
#define INFO(fmt, ...) (fprintf(stderr, "I " __FILE__ ": " fmt "\n", ##__VA_ARGS__))

extern void log_init(void);
extern void log_use_synchronous_mode(void);

#endif
