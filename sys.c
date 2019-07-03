#include <unistd.h>

#include "nrf_sdm.h"

#include "FreeRTOS.h"
#include "task.h"

extern void user_main(void*);
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
		"user", /* Text name for the task. */
		100, /* Stack depth in words. */
		(void *)500, /* Task parameters. */
		( 3 | portPRIVILEGE_BIT ), /* Priority and mode (Privileged in this case). */
		NULL /* Handle. */
	);
    xTaskCreate(
		user_main, /* The function that implements the task. */
		"user", /* Text name for the task. */
		100, /* Stack depth in words. */
		(void *)444, /* Task parameters. */
		( 3 | portPRIVILEGE_BIT ), /* Priority and mode (Privileged in this case). */
		NULL /* Handle. */
	);

	vTaskStartScheduler();

	/* Should not reach here as the scheduler is already started. */
	for( ; ; ) {
	}
}
