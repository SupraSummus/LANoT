#ifndef OS4CM4_THREAD_H
#define OS4CM4_THREAD_H

#include <os4cm4/cap.h>
#include <stdint.h>

#define THREAD_SIZE_SHIFT (7)
#define THREAD_SIZE (1 << THREAD_SIZE_SHIFT)

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
        // processor state, for context switching
        // 26 words * 4B = 104B
        struct regs_t regs;

        struct thread_t * waiting_for;
        struct thread_t * next_ready;

        // size 104B + 4B + 4B = 112B

        // capability root for this thread
        // It has to be 16B aligned. It is: 16B * 7 = 112B.
        struct cap_t cap;

        // size 128B
};

_Static_assert(sizeof(struct thread_t) == THREAD_SIZE);

extern struct thread_t * get_thread_by_id (uint32_t tid);
extern uint32_t get_thread_id (struct thread_t * t);
extern void request_switch (void);
extern struct thread_t * thread_new (
        struct thread_t *,
        void (* func) (uint32_t, uint32_t, uint32_t, uint32_t),
        void * stack,
        uint32_t stack_size,
        uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3
);

#endif
