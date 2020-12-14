#include <ARMCM4_FP.h>
#include <assert.h>
#include <os4cm4/printk.h>
#include <os4cm4/thread.h>
#include <stddef.h>
#include <string.h>


struct thread_t * thread_new (
        struct thread_t * thread,
        void (* func) (uint32_t, uint32_t, uint32_t, uint32_t),
        void * stack,
        uint32_t stack_size,
        uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3
) {
        struct thread_t * t = thread;
        assert(t->regs.sp == NULL);
        debug_printk(
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
