#include <unistd.h>
#include <stdio.h>

#include "nrf_sdm.h"
#include "app_error.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"

#include "FreeRTOS.h"
#include "task.h"
#include "usb_io.h"

extern void user_main(void*);
extern void user_main_write(void*);
extern void user_main_echo(void*);

extern uint32_t __isr_vector;

//unsigned long user_stack[256] __attribute__((aligned(1024))); 

/*
TaskParameters_t user_task_parameters = {
	user_main, // pvTaskCode
	"user", // pcName;
	256, // usStackDepth;
	NULL, // pvParameters
	1 | portPRIVILEGE_BIT, // priority
	user_stack, // puxStackBuffer;
	{
		{0, 0xffffffff, portMPU_REGION_READ_WRITE},
		{0, 0, 0},
		{0, 0, 0},
	}, // xRegions;
};
*/

int main(void) {
	ret_code_t ret;

	// disable sd
	sd_softdevice_disable();

	// set vtor
	SCB->VTOR = 0x00026000UL;
	__DSB();

	// enable clock needed by usbd
	ret = nrf_drv_clock_init();
	APP_ERROR_CHECK(ret);
	nrf_drv_clock_lfclk_request(NULL);
	while(!nrf_drv_clock_lfclk_is_running()) {
		/* Just waiting */
	}

	// enable usb serial communication
	usb_io_init();

	//printf("hello world\n");
	//printf("__isr_vector = %p\n", (void*)__isr_vector);
	//printf("SCB = %p\n", (void*)(SCB));
	//printf("SCB->VTOR = %p\n", (void*)(SCB->VTOR));

	xTaskCreate(
		user_main, /* The function that implements the task. */
		"user1", /* Text name for the task. */
		100, /* Stack depth in words. */
		(void *)500, /* Task parameters. */
		( 3 | portPRIVILEGE_BIT ), /* Priority and mode (Privileged in this case). */
		NULL /* Handle. */
	);
	xTaskCreate(
		user_main, /* The function that implements the task. */
		"user2", /* Text name for the task. */
		100, /* Stack depth in words. */
		(void *)444, /* Task parameters. */
		( 3 | portPRIVILEGE_BIT ), /* Priority and mode (Privileged in this case). */
		NULL /* Handle. */
	);
	xTaskCreate(
		user_main_write, /* The function that implements the task. */
		"user3", /* Text name for the task. */
		256, /* Stack depth in words. */
		NULL, /* Task parameters. */
		( 3 | portPRIVILEGE_BIT ), /* Priority and mode (Privileged in this case). */
		NULL /* Handle. */
	);
	xTaskCreate(
		user_main_echo, /* The function that implements the task. */
		"user4", /* Text name for the task. */
		256, /* Stack depth in words. */
		NULL, /* Task parameters. */
		( 3 | portPRIVILEGE_BIT ), /* Priority and mode (Privileged in this case). */
		NULL /* Handle. */
	);

	vTaskStartScheduler();

	/* Should not reach here as the scheduler is already started. */
	for( ; ; ) {
	}
}
