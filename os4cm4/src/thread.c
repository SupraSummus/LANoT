#include <ARMCM4_FP.h>
#include <assert.h>
#include <os4cm4/printk.h>
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

#define BACKGROUND_PRIO (0xff)

__attribute__((noreturn))
void kernel_start (void) {
        NVIC_SetPriorityGrouping(0);
        NVIC_EnableIRQ(SVCall_IRQn);
        NVIC_SetPriority(SVCall_IRQn, BACKGROUND_PRIO);
        NVIC_EnableIRQ(PendSV_IRQn);
        NVIC_SetPriority(PendSV_IRQn, BACKGROUND_PRIO);
        NVIC_EnableIRQ(MemoryManagement_IRQn);
        NVIC_SetPriority(MemoryManagement_IRQn, BACKGROUND_PRIO);
        NVIC_EnableIRQ(BusFault_IRQn);
        NVIC_SetPriority(BusFault_IRQn, BACKGROUND_PRIO);
        NVIC_EnableIRQ(UsageFault_IRQn);
        NVIC_SetPriority(UsageFault_IRQn, BACKGROUND_PRIO);

        struct thread_t this_thread;
        assert(current_thread == NULL);
        current_thread = &this_thread;
        printk("temporary start thread is %p\n", current_thread);
        kill_me();
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
        assert(t->regs.sp == NULL);
        printk(
                "thread_new %p, func %p, stack %p (size %#x), args %#x %#x %#x %#x\n",
                t, func, stack, stack_size,
                r0, r1, r2, r3
        );

        memset(t, 0, sizeof(struct thread_t));
        t->regs.lr = EXC_RETURN_THREAD_PSP;
        t->regs.sp = stack + stack_size - sizeof(struct exc_stack_t);

        memset(t->regs.sp, 0, sizeof(struct exc_stack_t));
        t->regs.sp->pc = (uint32_t)func;
        t->regs.sp->r0 = r0;
        t->regs.sp->r1 = r1;
        t->regs.sp->r2 = r2;
        t->regs.sp->r3 = r3;
        t->regs.sp->xpsr = xPSR_T_Msk;

        execute_later(t);

        return t;
}

/**
 * Ready list
 */

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
}
