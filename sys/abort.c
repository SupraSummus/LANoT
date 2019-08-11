#include "nrf_delay.h"

#include "led.h"

void abort (void) {
	while (true) {
		led_on();
		nrf_delay_ms(200);
		led_off();
		nrf_delay_ms(200);
	}
}
