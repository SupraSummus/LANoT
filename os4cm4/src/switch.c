#include <ARMCM4_FP.h>
#include <os4cm4/thread.h>
#include <stddef.h>

static struct thread_t * next_thread;
struct thread_t * current_thread;

void request_switch (struct thread_t * t) {
        next_thread = t;
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

#define save() \
        asm volatile (" \
                .extern current_thread \n\
                ldr r0, =current_thread \n\
                mrs r1, psp \n\
                stm r0!, {r1, r4-r11, lr} \n\
                // save floating point high registers only if process is using fpu \n\
                tst lr, #0x10 \n\
                it eq \n\
                vstmeq r0!, {s16-s31} \n\
        ");

#define restore() \
        asm volatile (" \
                .extern current_thread \n\
                ldr r0, =current_thread \n\
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
        current_thread = next_thread;
        next_thread = NULL;
        //next_thread = next_thread->next_ready;
        //next_thread->next_ready = 0;
        __enable_irq();
        restore();
        asm volatile ("bx lr");
}
