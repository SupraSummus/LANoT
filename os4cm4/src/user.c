void yield (void) {
        asm volatile ("svc #0");
}

void kill (int tid) {
        register int r0 asm ("r0") = tid;
        asm volatile ("svc #1");
}

__attribute__ ((noinline))
int rendezvous0 (int tid, int v) {
        asm volatile ("svc #2" ::: "r0", "r1");
}

__attribute__ ((noinline))
int rendezvous1 (int tid, int v, int * r) {
        asm volatile ("svc #2" ::: "r0", "r1");
        asm volatile ("str r1, [%0]" : "=r" (r));
}
