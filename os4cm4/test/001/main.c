#include <ARMCM4_FP.h>
#include <os4cm4/user/rendezvous.h>
#include <os4cm4/thread.h>

#define stack_size (1024)

static uint8_t task_1_stack[stack_size] __attribute__ ((aligned (stack_size)));
void task_1 (void) {
        for (int i = 0; i < 5; i ++) {
                int r;
                int from = rendezvous1(2, 10 + i, &r);
                //print("1 recv", hex(r), "from", hex(from));
        }
        rendezvous0(TID_KILLER, 1);
}

static uint8_t task_2_stack[stack_size] __attribute__ ((aligned (stack_size)));;
void task_2 (void) {
        for (int i = 0; i < 7; i ++) {
                int r;
                int ok = rendezvous1(2, 20 + i, &r);
                //print("2 recv", hex(r), "from", hex(ok));
        }
        rendezvous0(TID_KILLER, 2);
}

/*
static uint8_t task_3_stack[stack_size];
void task_3 (void) {

}

static uint8_t task_4_stack[stack_size];
void task_4 (void) {

}*/

void _start (void) {
        current_thread = threads + 0;
        struct thread_t * t = thread_new(
                1,
                task_1,
                task_1_stack,
                stack_size
        );
        thread_new(
                2,
                task_2,
                task_2_stack,
                stack_size
        );
        request_switch(t);
        while (1);
}

void __assert_func () {
        while (1);
}
