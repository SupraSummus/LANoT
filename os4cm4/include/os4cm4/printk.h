#pragma once

#define PRINTK_BUFFER_SIZE_SHIFT (10)
#define PRINTK_BUFFER_SIZE (1 << PRINTK_BUFFER_SIZE_SHIFT)
extern char printk_buffer[];

int printk (const char * format, ...);
void hex_dump (const char * desc, const void * addr, const int len);

//#define debug_printk printk
#define debug_printk(...) while (0) {}
