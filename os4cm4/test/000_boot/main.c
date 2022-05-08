#include <ARMCM4_FP.h>
#include <os4cm4/cap.h>
#include <os4cm4/kernel.h>
#include <os4cm4/mem.h>
#include <os4cm4/printk.h>
#include <os4cm4/thread.h>
#include <os4cm4/user.h>
#include <os4cm4/init.h>

#define ROOT_THREAD_CAP_TABLE_ENTRIES_SHIFT 4  // 2^4 entries
#include <os4cm4/default_static.c>

int main (void) {
        struct cap_t * caps_free = default_static_init();

        // last RAM region should be large and it contains the stack - allow RWX
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

        // check if default setup procedure allocated all ram
        int free_ram_size = 0;
        for (
                struct cap_t * cap = cap__cap_table__caps(&root_thread.cap);
                cap < caps_free;
                cap ++
        ) {
                if (cap_is_mem(cap)) {
                        free_ram_size += 1 << cap->size_shift;
                }
        }
        free_ram_size -= 1 << (10 + 10);  // account for flash
        extern const uint32_t end;
        extern const uint32_t __stack;
        printk(
                "%d\n",
                free_ram_size - ((uint32_t)&__stack - (uint32_t)&end)
        );  // should be 0

        printk("done\n");

        semihosting_end();
}
