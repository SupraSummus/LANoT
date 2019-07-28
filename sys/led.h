#ifndef LED_H
#define LED_H

#include "nrf_gpio.h"
#define LED_PIN (NRF_GPIO_PIN_MAP(0, 11))

static inline void led_on() {
	nrf_gpio_cfg_output(LED_PIN);
	nrf_gpio_pin_clear(LED_PIN);
}

static inline void led_off() {
	nrf_gpio_cfg_output(LED_PIN);
	nrf_gpio_pin_set(LED_PIN);
}

#endif
