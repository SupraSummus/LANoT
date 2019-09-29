/* redirect ISR routines to softdevice handlers */

#define SD_IRQ_ADDR(n) (0x00001000 + 4 * n)

	.globl POWER_CLOCK_IRQHandler
//	.type   SysTick_Handler, %function
POWER_CLOCK_IRQHandler:
	b SD_IRQ_ADDR(16)

RADIO_IRQHandler:
	b SD_IRQ_ADDR(17)

//    | (1U << RTC0_IRQn)
//    | (1U << TIMER0_IRQn)
//    | (1U << RNG_IRQn)
//    | (1U << ECB_IRQn)
//    | (1U << CCM_AAR_IRQn)
//    | (1U << TEMP_IRQn)
//    | (1U << __NRF_NVIC_NVMC_IRQn)
//    | (1U << (uint32_t)SWI5_IRQn)
