#ifndef LANOT_LOG_H
#define LANOT_LOG_H

#include <stdio.h>
#include <stdbool.h>

#include "FreeRTOSConfig.h"

/**
 * Log subsystem provides nonblocking, fast log stream. It's safe to use in IRQ.
 * It will drop bytes when writes are too fast though.
 */

#define LOG_TASK_PRIORITY (SYSTEM_TASK_PRIORITY)

#ifndef LOG_SUBSYSTEM
	#define LOG_SUBSYSTEM __FILE__
#endif

#define WARN(fmt, ...) (fprintf(stderr, "W " LOG_SUBSYSTEM ": " fmt "\n", ##__VA_ARGS__))
#define INFO(fmt, ...) (fprintf(stderr, "I " LOG_SUBSYSTEM ": " fmt "\n", ##__VA_ARGS__))

extern void log_init(void);
extern bool log_use_synchronous_mode (void);

extern int log_write(void * buf, size_t len);

#endif
