#include <stdio.h>

#include "log.h"

enum {r0, r1, r2, r3, r12, lr, pc, psr};

/*
void Hard_Fault_Handler(uint32_t stack[]) {
	static const int msg_len = 80;
	static char msg[msg_len];

	PANIC("In Hard Fault Handler");

	snprintf(msg, msg_len, "SCB->HFSR = 0x%08lx\n", SCB->HFSR);
	printf(msg);

	if ((SCB->HFSR & (1 << 30)) != 0) {
		printf("Forced Hard Fault\n");
		snprintf(msg, msg_len, "SCB->CFSR = 0x%08lx\n", SCB->CFSR);
		printf(msg);
		if((SCB->CFSR & 0xFFFF0000) != 0) {
			printUsageErrorMsg(SCB->CFSR);
		}
	}

	snprintf(msg, "r0  = 0x%08lx\n", stack[r0]);  printf(msg);
	snprintf(msg, "r1  = 0x%08lx\n", stack[r1]);  printf(msg);
	snprintf(msg, "r2  = 0x%08lx\n", stack[r2]);  printf(msg);
	snprintf(msg, "r3  = 0x%08lx\n", stack[r3]);  printf(msg);
	snprintf(msg, "r12 = 0x%08lx\n", stack[r12]); printf(msg);
	snprintf(msg, "lr  = 0x%08lx\n", stack[lr]);  printf(msg);
	snprintf(msg, "pc  = 0x%08lx\n", stack[pc]);  printf(msg);
	snprintf(msg, "psr = 0x%08lx\n", stack[psr]); printf(msg);
}

void HardFault_Handler (void) {
	asm volatile(
		" mrs r0,msp    \n"
		" b Hard_Fault_Handler \n"
	);
}
*/
