#include <os4cm4/svc.h>
#include <os4cm4/thread.h>
#include <os4cm4/printk.h>
#include <os4cm4/kernel.h>
#include <stddef.h>
#include <ARMCM4_FP.h>

inline static void do_yield (void) {
        execute_later(kernel.current_thread);
        request_switch();
}

inline static void do_kill_me (void) {
        // TODO mark thread slot as free.
        // setting pc = NULL is not enough as it will get overwritten during final context switch.
        //kernel.current_thread->regs.sp = NULL;
        request_switch();
}

void do_rendezvous (
        struct thread_t * current_thread,
        struct exc_stack_t * caller_stack,
        uint32_t ref,
        uint32_t v
) {
        uint32_t path = ref & 0xFFFFFFE0UL;
        unsigned int depth = ref & 0x0000001FUL;
        struct cap_t * cap = cap_find(
                &(current_thread->cap),
                path, depth
        );
        if (cap == NULL || cap->type != CAP_TYPE_THREAD) {
                caller_stack->r1 = -1;
                return;
        }
        struct thread_t * target_thread = cap_ptr_get(cap);

        if (target_thread == NULL || target_thread == current_thread) {
                // Tried to rendezvous with non-existent thread or self.
                caller_stack->r1 = -1;
                // It may be a userland malfunction.
                // Minimize damage (time lost) by switching to someone else.
                //execute_later(current_thread);
                //request_switch(pop_ready_thread());

        } else if (target_thread->waiting_for == current_thread) {
                // Target thread is waiting for us.
                // Do the rendezvous.
                caller_stack->r2 = target_thread->regs.sp->r2;
                target_thread->regs.sp->r2 = v;
                target_thread->waiting_for = NULL;
                current_thread->waiting_for = NULL;
                execute_later(target_thread);

        } else if (target_thread->waiting_for == NULL) {
                // Target thread is running.
                current_thread->waiting_for = target_thread;
                request_switch();

        } else {
                // Target thread is waiting for someone else.
                // Break their waiting and tell them we were calling.
                current_thread->waiting_for = target_thread;
                target_thread->waiting_for = NULL;
                //target_thread->regs.sp->r1 = get_thread_id(current_thread);
                target_thread->regs.sp->r2 = v;
                execute_later(target_thread);
                request_switch();
        }
}

void SVC_Handler (uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3) {
        struct exc_stack_t * caller_stack = (void *)__get_PSP();
        debug_printk("handling SVC %#x %#x %#x %#x, stack %p\n", r0, r1, r2, r3, caller_stack);
        switch (r0) {
                case SVC_YIELD:
                        do_yield();
                        break;
                case SVC_KILL_ME:
                        do_kill_me();
                        break;
                case SVC_RENDEZVOUS:
                        do_rendezvous(kernel.current_thread, caller_stack, r1, r2);
                        break;
                default:
                        caller_stack->r0 = -1;
                        break;
        }
        debug_printk("handling SVC end\n");
}
