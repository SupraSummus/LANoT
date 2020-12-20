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
        static struct cap_t * caps_free = root_thread_caps;
        cap_set_thread(
                caps_free,
                &root_thread
        );
        caps_free ++;

        // ram
        extern const uint32_t end;
        extern const uint32_t __stack;
        debug_printk("end %p, __stack %p\n", &end, &__stack);
        caps_free = mem_add(
                caps_free, root_thread_caps + 16,
                &end, &__stack + 1,
                MEM_PERM_R | MEM_PERM_W | MEM_PERM_X
        );
        mpu_map(
                &root_thread, 0,
                caps_free - 1
        );

        // flash
        cap_set_mem(
                caps_free,
                (void *)0x00000000,
                10 + 10,  // size = 2^(10 + 10)B = 1MiB
                MEM_PERM_R | MEM_PERM_X
        );
        mpu_map(
                &root_thread, 1,
                caps_free
        );
        caps_free ++;

        kernel_start(&root_thread);

        for (int i = 0; i < 10; i ++) {
                printk("main task yield\n");
                yield();
        }

        semihosting_end();
}
