#ifndef USER_TASK_H
#define USER_TASK_H

// TODO deduplicate constants with common/memory.ld
//#define user_flash ((void *)0x00040000)
#define user_flash_size (736 * 1024)
#define user_ram ((void *)0x20010000)
//#define user_ram_size (192 * 1024)

// fakes
#define user_flash ((void *)0x00050000)
#define user_ram_size (1 * 1024)

extern void user_task_start(void);
extern void user_task_kill(void);

#endif
