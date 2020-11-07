#ifndef LANOT_LOG_BACKEND_H
#define LANOT_LOG_BACKEND_H

#include <unistd.h>

/**
 * Log backend provides stream to push logs to. It may be slow, it may be
 * blocking.
 *
 * Log backend may be put into synchronous mode. Synchronous mode must push
 * logs without using interrupts. Once synchronous mode is enabled it can't
 * be disabled.
 */

extern ssize_t log_backend_write(const char *, size_t);
extern void log_backend_use_synchronous_mode(void);

#endif
