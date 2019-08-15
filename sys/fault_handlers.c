#include <stdlib.h>

#include "nrf52840.h"

#include "log.h"

enum {r0, r1, r2, r3, r12, lr, pc, psr};

void Hard_Fault_Handler(uint32_t stack[]) {
	log_use_synchronous_mode();
	INFO("in Hard_Fault_Handler");

	INFO("SCB->HFSR = 0x%08lx", SCB->HFSR);

	if ((SCB->HFSR & (1 << 30)) != 0) {
		INFO("Forced Hard Fault");
		INFO("SCB->CFSR = 0x%08lx", SCB->CFSR);
		//if((SCB->CFSR & 0xFFFF0000) != 0) {
		//	printUsageErrorMsg(SCB->CFSR);
		//}
	}

	INFO("r0  = 0x%08lx", stack[r0]);
	INFO("r1  = 0x%08lx", stack[r1]);
	INFO("r2  = 0x%08lx", stack[r2]);
	INFO("r3  = 0x%08lx", stack[r3]);
	INFO("r12 = 0x%08lx", stack[r12]);
	INFO("lr  = 0x%08lx", stack[lr]);
	INFO("pc  = 0x%08lx", stack[pc]);
	INFO("psr = 0x%08lx", stack[psr]);

	abort();
}

void HardFault_Handler (void) {
	__asm volatile(
		" mrs r0,msp    \n"
		" b Hard_Fault_Handler \n"
	);
}

void MemoryManagement_Handler (void) {
	log_use_synchronous_mode();
	INFO("in MemoryManagement_Handler");
	abort();
}

void BusFault_Handler (void) {
	log_use_synchronous_mode();
	INFO("in BusFault_Handler");
	abort();
}

void UsageFault_Handler (void) {
	log_use_synchronous_mode();
	INFO("in UsageFault_Handler");
	abort();
}
