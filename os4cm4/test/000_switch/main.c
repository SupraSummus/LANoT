#include <ARMCM4_FP.h>
#include <os4cm4/thread.h>
#include <os4cm4/user.h>
#include <os4cm4/printk.h>

#define stack_size (1024)
#define tasks_count (5)

static uint8_t task_stacks[stack_size * tasks_count] __attribute__ ((aligned (stack_size)));
static uint8_t kernel_stack[stack_size] __attribute__ ((aligned (stack_size)));

__attribute__((noreturn))
static void task (uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
        printk("thread started, args %i %i %i %i\n", a, b, c, d);
        yield();
        printk("thread after yield %i\n", a);
        kill_me();
}

int main (void) {
        for (int i = 0; i < tasks_count; i ++) {
                thread_new(
                        i,
                        task,
                        task_stacks + i * stack_size,
                        stack_size,
                        i, i * 7, 42, 0
                );
        }

        kernel_start(tasks_count, kernel_stack, stack_size);

        while (1) {
                printk("main task yield\n");
                yield();
        }
}
