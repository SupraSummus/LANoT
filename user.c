#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN (NRF_GPIO_PIN_MAP(0, 11))
//#define INTERVAL_MS (500)

void user_main(void * p) {
    uint32_t interval = (uint32_t)p;

	nrf_gpio_cfg_output(LED_PIN);
	nrf_gpio_pin_clear(LED_PIN);

	while (true) {
		vTaskDelay(pdMS_TO_TICKS(interval));
		//nrf_delay_ms(50);
		nrf_gpio_pin_toggle(LED_PIN);
	}
}
