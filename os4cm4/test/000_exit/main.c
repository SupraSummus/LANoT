#include <ARMCM4_FP.h>
#include <os4cm4/thread.h>
#include <os4cm4/user.h>
#include <os4cm4/printk.h>

#define stack_size (1024)
#define tasks_count (5)

static uint8_t task_stacks[stack_size * tasks_count] __attribute__ ((aligned (stack_size)));

__attribute__((noreturn))
static void task (uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
        printk("thread started, args %i %i %i %i\n", a, b, c, d);
        kill_me();
        printk("shouldn't reach here\n");
}

__attribute__((noreturn))
int main (void) {
        printk("start\n");

        for (int i = 0; i < tasks_count; i ++) {
                thread_new(
                        i,
                        task,
                        task_stacks + i * stack_size,
                        stack_size,
                        i, i * 7, 42, 0
                );
        }

        printk("before switch\n");
        kernel_start();
        printk("shouldn't reach here\n");
}
