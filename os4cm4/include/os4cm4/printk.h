#pragma once

int printk (const char * format, ...);
void hex_dump (const char * desc, const void * addr, const int len);

// #define debug_printk(...) (fprintf(stderr, "W " LOG_SUBSYSTEM ": " fmt "\n", ##__VA_ARGS__))
#define debug_printk(...) while (0);
