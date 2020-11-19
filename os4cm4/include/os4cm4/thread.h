#ifndef OS4CM4_THREAD_H
#define OS4CM4_THREAD_H

#include <stdint.h>

struct exc_stack_t {
        uint32_t r0;
        uint32_t r1;
        uint32_t r2;
        uint32_t r3;
        uint32_t r12;
        uint32_t lr;
        uint32_t pc;
        uint32_t xpsr;
        uint32_t fp_low[16];
};

struct regs_t {
        struct exc_stack_t * sp;
        uint32_t r4;
        uint32_t r5;
        uint32_t r6;
        uint32_t r7;
        uint32_t r8;
        uint32_t r9;
        uint32_t r10;
        uint32_t r11;
        uint32_t lr;
        uint32_t fp_high[16];
};

struct thread_t {
        struct regs_t regs;
        struct thread_t * waiting_for;
        struct thread_t * next_ready;
};

extern void kernel_start (void);

extern struct thread_t * current_thread;
extern struct thread_t * get_thread_by_id (uint32_t tid);
extern uint32_t get_thread_id (struct thread_t * t);
extern void request_switch (void);
extern struct thread_t * thread_new (
        uint32_t tid,
        void (* func) (uint32_t, uint32_t, uint32_t, uint32_t),
        void * stack,
        uint32_t stack_size,
        uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3
);
extern struct thread_t * pop_ready_thread (void);
extern void execute_later (struct thread_t *);

#endif
