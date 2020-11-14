#include <assert.h>
#include <os4cm4/thread.h>
#include <stddef.h>

/**
 * Thread registry
 *
 * Thread ids are for use in userspace. In the kernel pointers are preffered.
 */

#define MAX_THREADS (32)
struct thread_t threads[MAX_THREADS];

struct thread_t * get_thread_by_id (uint32_t tid) {
        if (tid >= MAX_THREADS) return NULL;
        struct thread_t * t = threads + tid;
        if (t->regs.sp == NULL) return NULL;
        return t;
}

uint32_t get_thread_id (struct thread_t * t) {
        return t - threads;
}

struct thread_t * thread_new (
        uint32_t tid,
        void (* func) (void),
        void * stack,
        uint32_t stack_size
) {
        assert(tid < MAX_THREADS);
        struct thread_t * t = threads + tid;
        assert(t->regs.sp != NULL);
        t->regs.sp = stack + stack_size - sizeof(struct exc_stack_t);
        t->regs.sp->pc = func;
        t->regs.lr = EXC_RETURN_THREAD_PSP;
        return t;
}

/**
 * Ready list
 */
/*
static struct thread_t * first_ready;
static struct thread_t * last_ready;

void execute_later (struct thread_t * t) {
        assert(t->next_ready == NULL);
        if (last_ready == NULL) {
                assert(first_ready == NULL);
                first_ready = t;
                last_ready = t;
        } else {
                last_ready->next_ready = t;
                last_ready = t;
        }
}

struct thread_t * pop_ready_thread (void) {
        assert(first_ready != NULL);
        struct thread_t * t = first_ready;
        first_ready = t->next_ready;
        t->next_ready = NULL;
        if (first_ready == NULL) last_ready = NULL;
        return t;
}*/
