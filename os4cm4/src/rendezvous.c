#include <os4cm4/thread.h>
#include <stddef.h>

static inline void mem_swap(uint32_t * a, uint32_t * b, unsigned int len) {
        #pragma GCC ivdep
        for (unsigned int i = 0; i < len; i ++) {
                uint32_t t = a[i];
                a[i] = b[i];
                b[i] = t;
        }
}

void SVC_Handler (void) {
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
                mem_swap(
                        (uint32_t *)target_thread->regs.sp + 1,
                        (uint32_t *)current_thread->regs.sp + 1,
                        3
                );
                target_thread->waiting_for = NULL;
                current_thread->waiting_for = NULL;
                //execute_later(target_thread);

        } else if (target_thread->waiting_for == NULL) {
                // Target thread is running.
                current_thread->waiting_for = target_thread;
                request_switch(target_thread);

        } else {
                // Target thread is waiting for someone else.
                // Break their waiting and tell them we were calling.
                current_thread->waiting_for = target_thread;
                target_thread->waiting_for = NULL;
                target_thread->regs.sp->r0 = get_thread_id(current_thread);
                request_switch(target_thread);
        }
}
