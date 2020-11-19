#include <os4cm4/user.h>
#include <os4cm4/svc.h>

void yield (void) {
        register uint32_t r0 __asm__ ("r0") = SVC_YIELD;
        __asm__ volatile ("svc #0" : "+r" (r0));
}

__attribute__((noreturn))
void kill_me (void) {
        register uint32_t r0 __asm__ ("r0") = SVC_KILL_ME;
        __asm__ volatile ("svc #0" : "+r" (r0));
        __builtin_unreachable();
}

uint32_t rendezvous (uint32_t tid, uint32_t out, uint32_t * in) {
        register uint32_t r0 __asm__ ("r0") = SVC_RENDEZVOUS;
        register uint32_t r1 __asm__ ("r1") = tid;
        register uint32_t r2 __asm__ ("r2") = out;
        __asm__ volatile ("svc #0" : "+r" (r0), "+r" (r1), "+r" (r2));
        *in = r2;
        return r1;
}
