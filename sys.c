#include <unistd.h>

#include "nrf_sdm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "usb_io.h"

extern void user_main(void*);
extern void user_main_write(void*);
extern void * __isr_vector;

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
	// disable sd
	sd_softdevice_disable();

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
	//xTaskCreate(
	//	user_main_write, /* The function that implements the task. */
	//	"user3", /* Text name for the task. */
	//	256, /* Stack depth in words. */
	//	NULL, /* Task parameters. */
	//	( 3 | portPRIVILEGE_BIT ), /* Priority and mode (Privileged in this case). */
	//	NULL /* Handle. */
	//);

	//usb_io_init();
	vTaskStartScheduler();

	/* Should not reach here as the scheduler is already started. */
	for( ; ; ) {
	}
}
