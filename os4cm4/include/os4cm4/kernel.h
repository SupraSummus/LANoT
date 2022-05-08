#pragma once

#include <os4cm4/thread.h>

#define KERNEL_MEM_SIZE_SHIFT (10)
#define KERNEL_MEM_SIZE (1 << KERNEL_MEM_SIZE_SHIFT)
#define BACKGROUND_PRIO (0xff)

struct kernel_t {
        char stack[KERNEL_MEM_SIZE - 4 * 3];
        struct thread_t * current_thread;
        struct thread_t * first_ready;
        struct thread_t * last_ready;
};

_Static_assert(sizeof(struct kernel_t) == KERNEL_MEM_SIZE);

extern struct kernel_t kernel;

/**
 * Initialize the kernel.
 * Control gets returned to the caller. Caller is now a root_thread and is running under the kernel control.
 */
void kernel_start (struct thread_t * root_thread);

/**
 * Ready list
 */

void execute_later (struct thread_t * t);
struct thread_t * pop_ready_thread (void);
