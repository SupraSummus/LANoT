#include <ARMCM4_FP.h>
#include <os4cm4/thread.h>
#include <os4cm4/user.h>
#include <stdio.h>

#define stack_size (1024)
#define tasks_count (5)

static uint8_t task_stacks[stack_size * tasks_count] __attribute__ ((aligned (stack_size)));

static void task (int tid) {
        printf("thread %i started\n", tid);
        kill(tid);
        printf("shouldn't reach here\n");
}

void _start (void) {
        printf("start\n");
        current_thread = threads + 0;

        for (int i = 0; i < tasks_count; i ++) {
                thread_new(
                        i + 1,
                        task,
                        task_stacks + i * stack_size,
                        stack_size
                );
        }

        printf("before switch\n");
        yield();
        printf("exiting\n");
        while (1);
        kill(0);
        printf("shouldn't reach here\n");
}
