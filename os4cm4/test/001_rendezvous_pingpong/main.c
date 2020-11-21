#include <ARMCM4_FP.h>
#include <os4cm4/user.h>
#include <os4cm4/thread.h>

#define stack_size (1024)
#define task_count (2)

static uint8_t task_stacks[stack_size * task_count] __attribute__ ((aligned (stack_size)));
static uint8_t kernel_stack[stack_size] __attribute__ ((aligned (stack_size)));

void task (uint32_t other_tid, uint32_t rendezvous_count, uint32_t base_value, uint32_t r3) {
        for (int i = 0; i < rendezvous_count; i ++) {
                int r;
                int from = rendezvous(other_tid, base_value + i, &r);
                printk("received %d from %d\n", r, from);
        }
        kill_me();
}

int main (void) {
        thread_new(
                1,
                task,
                task_stacks, stack_size,
                2, 2, 234, 0
        );
        thread_new(
                2,
                task,
                task_stacks + stack_size, stack_size,
                1, 4, 33, 0
        );
        kernel_start(0, kernel_stack, stack_size);

        for (int i = 0; i < 5; i ++) {
                printk("main task yield\n");
                yield();
        }

        semihosting_end();
}
