#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN (NRF_GPIO_PIN_MAP(0, 11))

void user_main(void * p) {
	uint32_t interval = (uint32_t)p;

	nrf_gpio_cfg_output(LED_PIN);
	nrf_gpio_pin_clear(LED_PIN);

	while (true) {
		vTaskDelay(pdMS_TO_TICKS(interval));
		nrf_gpio_pin_toggle(LED_PIN);
	}
}


void user_main_write(void * p) {
	(void)p;

	for (int i = 0; i++; ) {
		vTaskDelay(pdMS_TO_TICKS(1000));
		printf("write %d\n", i);
	}
}
