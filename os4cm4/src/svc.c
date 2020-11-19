#include <os4cm4/svc.h>
#include <os4cm4/thread.h>
#include <stddef.h>
#include <ARMCM4_FP.h>

static void do_yield (void) {
        printk("do_yield\n");
        execute_later(current_thread);
        request_switch();
}

static void do_kill_me (void) {
        // TODO mark thread slot as free.
        // setting pc = NULL is not enough as it will get overwritten during final context switch.
        //current_thread->regs.sp = NULL;
        request_switch();
}

static void do_rendezvous (void) {
        uint32_t target_thread_id = current_thread->regs.sp->r0;
        struct thread_t * target_thread = get_thread_by_id(target_thread_id);

        if (target_thread == NULL || target_thread == current_thread) {
                // Tried to rendezvous with non-existent thread or self.
                current_thread->regs.sp->r0 = -1;
                // It may be a userland malfunction.
                // Minimize damage (time lost) by switching to someone else.
                //execute_later(current_thread);
                //request_switch(pop_ready_thread());

        } else if (target_thread->waiting_for == current_thread) {
                // Target thread is waiting for us.
                // Do the rendezvous.
                uint32_t tmp = target_thread->regs.sp->r1;
                target_thread->regs.sp->r1 = current_thread->regs.sp->r1;
                current_thread->regs.sp->r1 = tmp;
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
                target_thread->regs.sp->r0 = get_thread_id(current_thread);
                target_thread->regs.sp->r1 = current_thread->regs.sp->r1;
                execute_later(target_thread);
                request_switch();
        }
}

void SVC_Handler (uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3) {
        struct exc_stack_t * caller_stack = __get_PSP();
        printk("handling SVC %#x %#x %#x %#x, stack %p\n", r0, r1, r2, r3, caller_stack);
        switch (r0) {
                case SVC_YIELD:
                        do_yield();
                        break;
                case SVC_KILL_ME:
                        do_kill_me();
                        break;
                case SVC_RENDEZVOUS:
                        do_rendezvous();
                        break;
                default:
                        caller_stack->r0 = -1;
                        break;
        }
        printk("handling SVC end\n");
}
