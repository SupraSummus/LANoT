#pragma once
#include <stdint.h>

__attribute__((noreturn))
extern void kill_me (void);

extern void yield (void);

extern uint32_t rendezvous (uint32_t tid, uint32_t out, uint32_t * in);
