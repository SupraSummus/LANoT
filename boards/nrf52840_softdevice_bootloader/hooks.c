#include "nrf_sdm.h"

#include "log.h"

void board_startup_hook(void) {
	// disable sd
	//sd_softdevice_disable();
	//INFO("SoftDevice disabled");

	// set vtor point to our mem
	// TODO deduplicate with memory.ld / linker script
	//SCB->VTOR = 0x00026000UL;
	//__DSB();
	//INFO("vector table relocated to %p", (void*)SCB->VTOR);
}
