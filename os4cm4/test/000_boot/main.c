#include <ARMCM4_FP.h>
#include <os4cm4/cap.h>
#include <os4cm4/kernel.h>
#include <os4cm4/mem.h>
#include <os4cm4/printk.h>
#include <os4cm4/thread.h>
#include <os4cm4/user.h>
#include <os4cm4/init.h>

struct kernel_t kernel __attribute__ ((aligned (KERNEL_MEM_SIZE)));
static struct thread_t root_thread __attribute__ ((aligned (THREAD_SIZE)));
static struct cap_t root_thread_caps[16] __attribute__ ((aligned (16 * CAP_SIZE)));
char printk_buffer[PRINTK_BUFFER_SIZE];

__attribute__((noreturn))
static void task (uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
        printk("thread started, args %i %i %i %i\n", a, b, c, d);
        yield();
        printk("thread after yield %i\n", a);
        kill_me();
}

int main (void) {
        cap_set_cap_table(
                &(root_thread.cap),
                root_thread_caps,
                4  // 2^4 entries
        );
        cap_set_thread(
                root_thread_caps + 0,
                &root_thread
        );

        // ram
        cap_set_mem(
                root_thread_caps + 1,
                (void *)0x20000000,
                10 + 8,  // size = 2^(10 + 8)B = 256KiB
                MEM_PERM_R | MEM_PERM_W | MEM_PERM_X
        );
        mpu_map(
                &root_thread, 0,
                root_thread_caps + 1
        );

        // flash
        cap_set_mem(
                root_thread_caps + 2,
                (void *)0x00000000,
                10 + 10,  // size = 2^(10 + 10)B = 1MiB
                MEM_PERM_R | MEM_PERM_X
        );
        mpu_map(
                &root_thread, 1,
                root_thread_caps + 2
        );

        mem_remove(
                root_thread_caps, root_thread_caps + 16,
                &kernel, sizeof(kernel)
        );
        mem_remove(
                root_thread_caps, root_thread_caps + 16,
                &root_thread, sizeof(root_thread)
        );
        mem_remove(
                root_thread_caps, root_thread_caps + 16,
                root_thread_caps, sizeof(root_thread_caps)
        );
        mem_remove(
                root_thread_caps, root_thread_caps + 16,
                printk_buffer, sizeof(printk_buffer)
        );

        kernel_start(&root_thread);

        for (int i = 0; i < 10; i ++) {
                printk("main task yield\n");
                yield();
        }

        semihosting_end();
}
