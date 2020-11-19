#include <ARMCM4_FP.h>
#include <os4cm4/thread.h>
#include <stddef.h>

struct thread_t * current_thread;

void request_switch (void) {
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

#define save() \
        __asm__ volatile (" \
                .extern current_thread \n\
                ldr r0, =current_thread \n\
                ldr r0, [r0] \n\
                //mov r0, current_thread \n\
                mrs r1, psp \n\
                stm r0!, {r1, r4-r11, lr} \n\
                // save floating point high registers only if process is using fpu \n\
                tst lr, #0x10 \n\
                it eq \n\
                vstmeq r0!, {s16-s31} \n\
        ");

#define restore() \
        __asm__ volatile (" \
                .extern current_thread \n\
                ldr r0, =current_thread \n\
                ldr r0, [r0] \n\
                //mov r0, current_thread \n\
                ldm r0!, {r1, r4-r11, lr} \n\
                msr psp, r1 \n\
                // load floating point high registers only if process is using fpu \n\
                tst lr, #0x10 \n\
                it eq \n\
                vldmeq r0!, {s16-s31} \n\
        ");

void __attribute__((naked)) PendSV_Handler (void) {
        save();
        __disable_irq();
        printk("switching threads: %p", current_thread);
        //hex_dump("old thread", current_thread, sizeof(struct thread_t));
        current_thread = pop_ready_thread();
        printk(" to %p\n", current_thread);
        //hex_dump("new thread", current_thread, sizeof(struct thread_t));
        //hex_dump("new thread stack", current_thread->regs.sp, sizeof(struct exc_stack_t));
        __enable_irq();
        restore();
        __asm__ volatile ("bx lr");
}
