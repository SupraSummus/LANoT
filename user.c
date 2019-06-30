#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN (NRF_GPIO_PIN_MAP(0, 11))
#define INTERVAL_MS (500)

void user_main(void) {
	nrf_gpio_cfg_output(LED_PIN);
	nrf_gpio_pin_clear(LED_PIN);

	while (true) {
		nrf_delay_ms(INTERVAL_MS);
		nrf_gpio_pin_toggle(LED_PIN);
	}
}
