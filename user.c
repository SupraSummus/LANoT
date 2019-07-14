#include <unistd.h>

#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "syscall.h"

#define LED_PIN (NRF_GPIO_PIN_MAP(0, 11))

void user_main(void * p) {
	uint32_t interval = (uint32_t)p;

	nrf_gpio_cfg_output(LED_PIN);
	nrf_gpio_pin_clear(LED_PIN);

	while (true) {
		int ret = syscall(1 /* delay ms */, interval, 0, 0);
		printf("syscall ret %d\n", ret);
		nrf_gpio_pin_toggle(LED_PIN);
	}
}


void user_main_write(void * p) {
	(void)p;

	for (int i = 0; true; i++) {
		vTaskDelay(pdMS_TO_TICKS(1000));
		printf("write %d\n", i);
	}
}

void user_main_echo(void * p) {
	(void)p;
	char buf[16];

	while (true) {
		ssize_t s = read(STDIN_FILENO, buf, 15);
		if (s > 0) {
			buf[s] = '\0';
			printf("got >>>%s<<<\n", buf);
		} else {
			printf("read returned %d\n", s);
		}
	}
}
