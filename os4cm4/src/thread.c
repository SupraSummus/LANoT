#include <ARMCM4_FP.h>
#include <assert.h>
#include <os4cm4/thread.h>
#include <stddef.h>
#include <string.h>

/**
 * Thread registry
 *
 * Thread ids are for use in userspace. In the kernel pointers are preffered.
 */

#define MAX_THREADS 32
static struct thread_t threads[MAX_THREADS];

__attribute__((noreturn))
void kernel_start (void) {

}

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
        void (* func) (uint32_t, uint32_t, uint32_t, uint32_t),
        void * stack,
        uint32_t stack_size,
        uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3
) {
        assert(tid < MAX_THREADS);
        struct thread_t * t = threads + tid;
        printk("threads %p, size %i\n", threads, sizeof(threads));
        printk("sp %p\n", t->regs.sp);
        assert(t->regs.sp == NULL);

        memset(t, 0, sizeof(struct thread_t));
        t->regs.lr = EXC_RETURN_THREAD_PSP;
        t->regs.sp = stack + stack_size - sizeof(struct exc_stack_t);

        memset(t->regs.sp, 0, sizeof(struct exc_stack_t));
        t->regs.sp->pc = (uint32_t)func;
        t->regs.sp->r0 = r0;
        t->regs.sp->r1 = r1;
        t->regs.sp->r2 = r2;
        t->regs.sp->r3 = r3;

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
