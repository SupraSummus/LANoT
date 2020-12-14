#include <ARMCM4_FP.h>
#include <assert.h>
#include <os4cm4/kernel.h>
#include <os4cm4/printk.h>
#include <stddef.h>

void kernel_start (struct thread_t * root_thread) {
        if (!(__get_CONTROL() & CONTROL_SPSEL_Msk)) {
                // using Main SP? switch to Process SP
                __set_PSP(__get_MSP());
                __set_CONTROL(__get_CONTROL() | CONTROL_SPSEL_Msk);
        }

        debug_printk("kernel stack at %p, size %#x\n", &(kernel.stack), sizeof(kernel.stack));
        __set_MSP((uint32_t)kernel.stack + sizeof(kernel.stack));
        kernel.current_thread = root_thread;

        debug_printk("enabling kernel IRQs\n");
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
}

/**
 * Ready list
 */

void execute_later (struct thread_t * t) {
        assert(t->next_ready == NULL);
        if (kernel.last_ready == NULL) {
                assert(kernel.first_ready == NULL);
                kernel.first_ready = t;
                kernel.last_ready = t;
        } else {
                kernel.last_ready->next_ready = t;
                kernel.last_ready = t;
        }
}

struct thread_t * pop_ready_thread (void) {
        assert(kernel.first_ready != NULL);
        struct thread_t * t = kernel.first_ready;
        kernel.first_ready = t->next_ready;
        t->next_ready = NULL;
        if (kernel.first_ready == NULL) kernel.last_ready = NULL;
        return t;
}
