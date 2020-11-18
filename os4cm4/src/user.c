#include <os4cm4/user.h>

void yield (void) {
        __asm__ volatile ("svc #0");
}

__attribute__((noreturn))
void kill_me (void) {
        __asm__ volatile ("svc #1");
        __builtin_unreachable();
}

uint32_t rendezvous (uint32_t tid, uint32_t out, uint32_t * in) {
        register uint32_t r0 __asm__ ("r0") = tid;
        register uint32_t r1 __asm__ ("r1") = out;
        __asm__ volatile ("svc #2");
        *in = r1;
        return r0;
}
