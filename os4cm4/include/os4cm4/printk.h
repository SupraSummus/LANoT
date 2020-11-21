#pragma once

int printk (const char * format, ...);
void hex_dump (const char * desc, const void * addr, const int len);

//#define debug_printk printk
#define debug_printk(...) while (0) {}
